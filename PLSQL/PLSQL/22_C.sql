--1

CREATE OR REPLACE FUNCTION IS_READY_FOR_PROMOTION (
    P_EMPLOYEE_ID IN EMPLOYEES.EMPLOYEE_ID%TYPE
) RETURN VARCHAR2
IS
    v_hire_date   EMPLOYEES.HIRE_DATE%TYPE;
    v_salary      EMPLOYEES.SALARY%TYPE;
    v_job_min     JOBS.MIN_SALARY%TYPE;
    v_job_max     JOBS.MAX_SALARY%TYPE;
    v_sub_count   NUMBER;
    v_mid_salary  NUMBER;
BEGIN
    -- Retrieve employee info and job salary range
    SELECT E.HIRE_DATE, E.SALARY, J.MIN_SALARY, J.MAX_SALARY
    INTO v_hire_date, v_salary, v_job_min, v_job_max
    FROM EMPLOYEES E
    JOIN JOBS J ON E.JOB_ID = J.JOB_ID
    WHERE E.EMPLOYEE_ID = P_EMPLOYEE_ID;

    -- Calculate midpoint of job salary range
    v_mid_salary := (v_job_min + v_job_max) / 2;

    -- Count subordinates
    SELECT COUNT(*)
    INTO v_sub_count
    FROM EMPLOYEES
    WHERE MANAGER_ID = P_EMPLOYEE_ID;

    -- Check promotion criteria
    IF MONTHS_BETWEEN(SYSDATE, v_hire_date) >= 60 -- 5 years = 60 months
       AND v_salary > v_mid_salary
       AND v_sub_count >= 1
    THEN
        RETURN 'YES';
    ELSE
        RETURN 'NO';
    END IF;

EXCEPTION
    WHEN NO_DATA_FOUND THEN
        RETURN 'Employee not found';
    WHEN OTHERS THEN
        RETURN 'Error: ' || SQLERRM;
END;
/

--2
-- Copy EMPLOYEES to EMPLOYEES_COPY
CREATE TABLE EMPLOYEES_COPY AS
SELECT * FROM EMPLOYEES;

CREATE OR REPLACE PROCEDURE UPDATE_EMPLOYEES_SALARY IS
    -- Variables to hold employee info
    v_emp_id        EMPLOYEES_COPY.EMPLOYEE_ID%TYPE;
    v_old_salary    EMPLOYEES_COPY.SALARY%TYPE;
    v_new_salary    NUMBER;
    v_comm_pct      EMPLOYEES_COPY.COMMISSION_PCT%TYPE;
    v_min_salary    JOBS.MIN_SALARY%TYPE;
    v_max_salary    JOBS.MAX_SALARY%TYPE;
    v_avg_dept_sal  NUMBER;
    v_hire_date     EMPLOYEES_COPY.HIRE_DATE%TYPE;
    v_today         DATE;
BEGIN
    -- Find the newest employee hire date (used as "today")
    SELECT MAX(HIRE_DATE) INTO v_today FROM EMPLOYEES_COPY;

    -- Cursor to loop through all employees
    FOR rec IN (SELECT * FROM EMPLOYEES_COPY) LOOP
        BEGIN
            v_emp_id := rec.EMPLOYEE_ID;
            v_old_salary := rec.SALARY;
            v_comm_pct := NVL(rec.COMMISSION_PCT, 0);
            v_hire_date := rec.HIRE_DATE;

            -- Skip employees with 1 year or less
            IF MONTHS_BETWEEN(v_today, v_hire_date) <= 12 THEN
                CONTINUE;
            END IF;

            -- Get job min/max salary
            SELECT MIN_SALARY, MAX_SALARY
            INTO v_min_salary, v_max_salary
            FROM JOBS
            WHERE JOB_ID = rec.JOB_ID;

            -- Get average salary of employee's department
            SELECT AVG(SALARY)
            INTO v_avg_dept_sal
            FROM EMPLOYEES_COPY
            WHERE DEPARTMENT_ID = rec.DEPARTMENT_ID;

            -- Calculate new salary
            v_new_salary := v_old_salary 
                            + (v_comm_pct * v_old_salary)
                            + (0.1 * v_min_salary)
                            + (0.1 * v_avg_dept_sal);

            -- If new salary exceeds job max salary, cap it
            IF v_new_salary > v_max_salary THEN
                v_new_salary := v_max_salary;
            END IF;

            -- Update salary
            UPDATE EMPLOYEES_COPY
            SET SALARY = v_new_salary
            WHERE EMPLOYEE_ID = v_emp_id;

        EXCEPTION
            WHEN OTHERS THEN
                DBMS_OUTPUT.PUT_LINE('Error updating employee ' || rec.EMPLOYEE_ID 
                                     || ': ' || SQLERRM);
        END;
    END LOOP;

    COMMIT;
    DBMS_OUTPUT.PUT_LINE('Salary update completed.');
END;
/

BEGIN
    UPDATE_EMPLOYEES_SALARY;
END;
/

-- Show old vs new salaries
SELECT e.employee_id, e.salary AS Old_Salary, ec.salary AS New_Salary,
       j.min_salary, j.max_salary, e.hire_date
FROM employees_copy ec
JOIN employees e ON ec.employee_id = e.employee_id
JOIN jobs j ON e.job_id = j.job_id
ORDER BY e.employee_id;




--3
CREATE TABLE Demotions(
    EMPLOYEE_ID NUMBER,
    CURRENT_SALARY NUMBER,
    STATUS VARCHAR2(20),
    DEMOTION_DATE DATE
);

CREATE OR REPLACE TRIGGER SALARY_UPDATE_DEMOTION
AFTER UPDATE OF SALARY ON EMPLOYEES
FOR EACH ROW
DECLARE
    v_newest_hire_date DATE;
    v_old_salary EMPLOYEES.SALARY%TYPE;
    v_new_salary EMPLOYEES.SALARY%TYPE;
    v_decrease_pct NUMBER;
    v_highest_paid_emp EMPLOYEES.EMPLOYEE_ID%TYPE;
BEGIN
    -- Use hire date of newest employee as current date
    SELECT MAX(HIRE_DATE) INTO v_newest_hire_date FROM EMPLOYEES;

    v_old_salary := :OLD.SALARY;
    v_new_salary := :NEW.SALARY;

    -- Calculate percentage decrease
    IF v_new_salary < v_old_salary THEN
        v_decrease_pct := (v_old_salary - v_new_salary) / v_old_salary * 100;
    ELSE
        v_decrease_pct := 0;
    END IF;

    -- If decrease > 20%, handle demotion
    IF v_decrease_pct > 20 THEN
        -- Insert into Demotions table with status 'waiting'
        INSERT INTO Demotions (EMPLOYEE_ID, CURRENT_SALARY, STATUS, DEMOTION_DATE)
        VALUES (:OLD.EMPLOYEE_ID, v_new_salary, 'waiting', v_newest_hire_date);

        -- Check if employee is a manager
        IF EXISTS (SELECT 1 FROM EMPLOYEES WHERE MANAGER_ID = :OLD.EMPLOYEE_ID) THEN
            BEGIN
                -- Find the highest-paid employee under this manager
                SELECT EMPLOYEE_ID
                INTO v_highest_paid_emp
                FROM (
                    SELECT EMPLOYEE_ID
                    FROM EMPLOYEES
                    WHERE MANAGER_ID = :OLD.EMPLOYEE_ID
                    ORDER BY SALARY DESC
                )
                WHERE ROWNUM = 1;

                -- Switch manager of the highest-paid employee to the demoted employee's manager
                UPDATE EMPLOYEES
                SET MANAGER_ID = :OLD.MANAGER_ID
                WHERE EMPLOYEE_ID = v_highest_paid_emp;

                -- Update the demoted manager to manage the highest-paid employee
                UPDATE EMPLOYEES
                SET MANAGER_ID = v_highest_paid_emp
                WHERE EMPLOYEE_ID = :OLD.EMPLOYEE_ID;

                -- Optionally mark demotion done
                UPDATE Demotions
                SET STATUS = 'done'
                WHERE EMPLOYEE_ID = :OLD.EMPLOYEE_ID;

            EXCEPTION
                WHEN NO_DATA_FOUND THEN
                    -- No subordinates, keep status as waiting
                    NULL;
            END;
        END IF;
    END IF;

EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Error in SALARY_UPDATE_DEMOTION trigger: ' || SQLERRM);
END;
/