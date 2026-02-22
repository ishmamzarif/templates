SET SERVEROUTPUT ON;

BEGIN
   DBMS_OUTPUT.PUT_LINE('hello');
END;
/


--22 BATCH
-- 22 A
-- Question 1.
-- Write a PL/SQL procedure named LONGEST_SERVING_EMPLOYEE that takes a
-- REGION_NAME as input and identifies the employee who has been working for the longest time within
-- that region.
-- Retrieve and display the following details for that employee.
-- • Full Name (First Name || Last Name)
-- • Job Title
-- • Hire Date
-- • Country Name
-- • Department Name
-- • City
-- Make sure to handle exceptions with appropriate messages.
CREATE OR REPLACE PROCEDURE LONGEST_SERVING_EMPLOYEE (
    p_region_name IN VARCHAR2
) 
IS
    v_full_name      VARCHAR2(100);
    v_job_title      jobs.job_title%TYPE;
    v_hire_date      employees.hire_date%TYPE;
    v_country_name   countries.country_name%TYPE;
    v_department     departments.department_name%TYPE;
    v_city           locations.city%TYPE;
BEGIN
    -- Query to find the longest-serving employee in the given region
    SELECT full_name, job_title, hire_date, country_name, department_name, city
    INTO v_full_name, v_job_title, v_hire_date, v_country_name, v_department, v_city
    FROM (
        SELECT e.first_name || ' ' || e.last_name AS full_name,
               j.job_title,
               e.hire_date,
               c.country_name,
               d.department_name,
               l.city
        FROM employees e
        JOIN departments d ON e.department_id = d.department_id
        JOIN locations l   ON d.location_id = l.location_id
        JOIN countries c   ON l.country_id = c.country_id
        JOIN regions r     ON c.region_id = r.region_id
        JOIN jobs j        ON e.job_id = j.job_id
        WHERE UPPER(r.region_name) = UPPER(p_region_name)
        ORDER BY e.hire_date ASC
    )
    WHERE ROWNUM = 1; -- Ensures we only get the single earliest hired employee

    -- Displaying the employee details
    DBMS_OUTPUT.PUT_LINE('--------------------------------------------------');
    DBMS_OUTPUT.PUT_LINE('Longest Serving Employee in ' || UPPER(p_region_name));
    DBMS_OUTPUT.PUT_LINE('--------------------------------------------------');
    DBMS_OUTPUT.PUT_LINE('Full Name       : ' || v_full_name);
    DBMS_OUTPUT.PUT_LINE('Job Title       : ' || v_job_title);
    DBMS_OUTPUT.PUT_LINE('Hire Date       : ' || TO_CHAR(v_hire_date, 'DD-MON-YYYY'));
    DBMS_OUTPUT.PUT_LINE('Country Name    : ' || v_country_name);
    DBMS_OUTPUT.PUT_LINE('Department Name : ' || v_department);
    DBMS_OUTPUT.PUT_LINE('City            : ' || v_city);
    DBMS_OUTPUT.PUT_LINE('--------------------------------------------------');
    RAISE_APPLICATION_ERROR(-20001,'ERROR_LOL');

EXCEPTION
    WHEN NO_DATA_FOUND THEN
        DBMS_OUTPUT.PUT_LINE('Error: No employees found for the region "' || p_region_name || '" or the region does not exist.');
    WHEN TOO_MANY_ROWS THEN
        DBMS_OUTPUT.PUT_LINE('HOISE KICHU EKTA');
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('An unexpected error occurred: ' || SQLERRM);
END;
/

DECLARE
BEGIN
  LONGEST_SERVING_EMPLOYEE('Europe');
END;
/

-- Question 2.
-- Write a procedure named RANK_JOBS in Oracle HR schema. The procedure should rank jobs based on
-- the following criteria:
-- 1. Number of Employees: Jobs should be ranked in descending order of their number of
-- employees. The job with the highest number of employees should be ranked as 1.
-- 2. Average Salary: If multiple jobs have the same number of employees, the ranking should be
-- determined based on the average salary in that job, in descending order.
-- The output should display the following information:

-- o Rank
-- o Job Title
-- o Total Number of Employees
-- o Average Salary
-- o Maximum Salary
-- o Minimum Salary
CREATE OR REPLACE PROCEDURE RANK_JOBS AS
    i NUMBER;
BEGIN 
    i := 1;
    FOR R IN (
        SELECT JOB_TITLE, COUNT(EMPLOYEE_ID) NUMEMPS, AVG(SALARY) AVSAL, MAX(SALARY) MAXSAL, MIN(SALARY) MINSAL
        FROM
            EMPLOYEES E JOIN JOBS J ON (E.JOB_ID = J.JOB_ID)
        GROUP BY
            JOB_TITLE
        ORDER BY
            COUNT(EMPLOYEE_ID) DESC, AVG(SALARY) DESC
    )
    LOOP
        DBMS_OUTPUT.PUT_LINE('RANK: ' || TO_CHAR(i)); 
        DBMS_OUTPUT.PUT_LINE('--------------------');
        DBMS_OUTPUT.PUT_LINE('JOB TITLE: ' || TO_CHAR(R.JOB_TITLE));
        DBMS_OUTPUT.PUT_LINE('EMPLOYEES: '|| TO_CHAR(R.NUMEMPS));
        DBMS_OUTPUT.PUT_LINE('AVG SALLARY: '|| TO_CHAR(R.AVSAL));
        DBMS_OUTPUT.PUT_LINE('MAX SALARY: ' || TO_CHAR(R.MAXSAL));
        DBMS_OUTPUT.PUT_LINE('MIN SALARY: ' || TO_CHAR(R.MINSAL));

        i := i + 1;        
    END LOOP;

END;
/

DECLARE
BEGIN
    RANK_JOBS;
END; 
/

-- Question 3.
-- Create a trigger that activates when an employee leaves the job (i.e., when a DELETE operation is
-- performed on the Employee table).
-- 1 new table: Leaves
-- (fields:
-- employee_id,
-- employee_working_instead_of_him/her,
-- current date
-- )
-- Conditions:
-- 1. If that employee has a manager, his/her work should be done by the employee who has the same
-- manager and has the closest salary to him\her.
-- 2. In case the employee is a manager, his/her work is done by a manager who has the closest
-- subordinate count as him/her.
-- 3. If an employee meets both conditions 1 and 2 (is a manager and has a manager), go for 2.
-- 4. If no substitute employee is found, keep that field null.
-- No changes in the Job table and the Job_history table are necessary for your ease.
CREATE TABLE LEAVES (
    EMPLOYEE_ID NUMBER PRIMARY KEY;
    REPLACED EMPLOYEE_ID NUMBER,
    LEAVING_DATE DATE
);

CREATE OR REPLACE TRIGGER trg_employee_leaves
BEFORE DELETE ON employees
FOR EACH ROW
DECLARE
    -- This pragma is required to prevent the "Mutating Table" error
    -- since we are querying the same table we are deleting from.
    PRAGMA AUTONOMOUS_TRANSACTION; 
    
    v_subordinate_count NUMBER := 0;
    v_substitute_id     NUMBER := NULL;
BEGIN
    -- First, check how many subordinates the leaving employee has (to see if they are a manager)
    SELECT COUNT(*)
    INTO v_subordinate_count
    FROM employees
    WHERE manager_id = :OLD.employee_id;

    -- CONDITIONS 2 & 3: Employee is a manager (has subordinates)
    IF v_subordinate_count > 0 THEN
        BEGIN
            SELECT manager_id INTO v_substitute_id
            FROM (
                SELECT manager_id, COUNT(employee_id) AS sub_count
                FROM employees
                WHERE manager_id IS NOT NULL 
                  AND manager_id != :OLD.employee_id -- Exclude the leaving manager
                GROUP BY manager_id
                -- Order by the smallest difference in subordinate count
                ORDER BY ABS(COUNT(employee_id) - v_subordinate_count) ASC, manager_id ASC
            )
            WHERE ROWNUM = 1;
        EXCEPTION
            WHEN NO_DATA_FOUND THEN
                v_substitute_id := NULL; -- Condition 4: No substitute found
        END;

    -- CONDITION 1: Regular employee with a manager
    ELSIF :OLD.manager_id IS NOT NULL THEN
        BEGIN
            SELECT employee_id INTO v_substitute_id
            FROM (
                SELECT employee_id
                FROM employees
                WHERE manager_id = :OLD.manager_id 
                  AND employee_id != :OLD.employee_id -- Exclude the leaving employee
                -- Order by the smallest difference in salary
                ORDER BY ABS(salary - :OLD.salary) ASC, employee_id ASC
            )
            WHERE ROWNUM = 1;
        EXCEPTION
            WHEN NO_DATA_FOUND THEN
                v_substitute_id := NULL; -- Condition 4: No substitute found
        END;
    END IF;

    -- Insert the record into the Leaves table
    INSERT INTO Leaves (employee_id, substitute_employee_id, leave_date)
    VALUES (:OLD.employee_id, v_substitute_id, SYSDATE);

    -- An autonomous transaction must be committed before it finishes
    COMMIT; 
END;
/



--22 B
-- Question 1.
-- First create a copy of the EMPLOYEES table in the HR schema, named EMPLOYEES_COPY. Do all
-- the work in EMPLOYEES_COPY table.
-- Write a function named Exchange_Employees that should take two manager_ids as parameters and
-- exchange the lowest-paying employees under each manager. If there is a tie (i.e., multiple employees
-- have the same salary), select any one of them. Note that when employees are exchanged, their jobs will
-- remain the same, but one employee will join the department of the other employee under that employee’s
-- manager. The salaries of the exchanged employees will be updated. The new salary for each employee
-- will be increased by 50% of the difference between their original salaries. Print the employee’s
-- information before and after the exchange and handle appropriate exceptions.
-- 
-- Before EXCHANGE_EMPLOYEES(100,145):
-- Employee Information:
-- Employee ID: 124 Name: Kevin Mourgos
-- Email: KMOURGOS Phone Number: 650.123.5234
-- Hire Date: 16-NOV-07
-- Job ID: ST_MAN Salary: 5800
-- Commission Percentage:
-- Manager ID: 100 Department ID: 50
-- Employee Information:.

-- Employee ID: 155 Name: Oliver Tuvault
-- Email: OTUVAULT Phone Number: 011.44.1344.486508
-- Hire Date: 23-NOV-07
-- Job ID: SA_REP Salary: 7000
-- Commission Percentage: .15
-- Manager ID: 145 Department ID: 80

-- After EXCHANGE_EMPLOYEES(100,145):
-- Employee Information:
-- Employee ID: 124 Name: Kevin Mourgos
-- Email: KMOURGOS Phone Number: 650.123.5234
-- Hire Date: 16-NOV-07
-- Job ID: ST_MAN Salary: 6400
-- Commission Percentage:
-- Manager ID: 145 Department ID: 80
-- Employee Information:
-- Employee ID: 155 Name: Oliver Tuvault
-- Email: OTUVAULT Phone Number: 011.44.1344.486508
-- Hire Date: 23-NOV-07
-- Job ID: SA_REP Salary: 7600
-- Commission Percentage: .15
-- Manager ID: 100 Department ID: 50

-- To copy a table, execute the following script-
CREATE TABLE employees_copy AS
SELECT * FROM employees;
COMMIT;

CREATE OR REPLACE FUNCTION exchange_employees (
    mid1 IN NUMBER,
    mid2 IN NUMBER
) RETURN VARCHAR2
IS
    emp_id1 NUMBER;
    dept1 EMPLOYEES_COPY.department_id%TYPE;
    sal1 EMPLOYEES_COPY.salary%TYPE;
    emp_id2 NUMBER;
    dept2 EMPLOYEES_COPY.department_id%TYPE;
    sal2 EMPLOYEES_COPY.salary%TYPE;
BEGIN
    SELECT employee_id, department_id, salary
    INTO emp_id1, dept1, sal1
    FROM (
        SELECT employee_id, department_id, salary
        FROM EMPLOYEES_COPY
        WHERE manager_id = mid1
        ORDER BY salary ASC
    )
    WHERE ROWNUM = 1;
    SELECT employee_id, department_id, salary
    INTO emp_id2, dept2, sal2
    FROM (
        SELECT employee_id, department_id, salary
        FROM EMPLOYEES_COPY
        WHERE manager_id = mid2
        ORDER BY salary ASC
    )
    WHERE ROWNUM = 1; 

--info before exchange
    show_employee_info(emp_id1);
    show_employee_info(emp_id2);
    
    UPDATE EMPLOYEES_COPY
    SET department_id = dept2,
    manager_id = mid2,
    salary=abs(sal1-sal2)*0.5+salary
    WHERE employee_id = emp_id1;
    UPDATE EMPLOYEES_COPY
    SET department_id = dept1,
    manager_id = mid1,
    salary=abs(sal1-sal2)*0.5+salary
    WHERE employee_id = emp_id2;
    COMMIT;

--info after exchange
-- show_employee_info(emp_id1);
-- show_employee_info(emp_id2);
RETURN 'Employees have been successfully exchanged.';
EXCEPTION
WHEN NO_DATA_FOUND THEN
RETURN 'One or both employee IDs do not exist.';
WHEN OTHERS THEN
RETURN 'An error occurred during the exchange process.';
END;
/

-- Question 2.
-- Write a PL/SQL procedure named LOCATION_SALARY_REPORT that performs the following
-- tasks:
--     • For each location (city), compute:
--           o The number of employees working in that city
--           o The average salary of those employees (rounded to 2 decimal places)
--           o The job title of the highest-paid employee in that city
--     • Rank the cities based on:
--           o Ascending order of the number of employees
--           o Descending order of average salary (used as a tie-breaker)
--     • Print the following information for each location:
--           o Rank
--           o City Name
--           o Number of Employees
--           o Average Salary
--           o Highest Paying Job Title
    
CREATE OR REPLACE PROCEDURE LOCATION_SALARY_REPORT IS
    -- Variable to track the rank manually
    V_RANK NUMBER := 0;
    V_JOB_TITLE VARCHAR2(50);
BEGIN
    -- Header for the report
    DBMS_OUTPUT.PUT_LINE(RPAD('RANK', 6) || RPAD('CITY', 15) || RPAD('EMPS', 8) || RPAD('AVG SALARY', 12) || 'TOP JOB');
    DBMS_OUTPUT.PUT_LINE('----------------------------------------------------------------------');

    FOR X IN (
        SELECT 
            L.CITY,
            L.LOCATION_ID,
            COUNT(E.EMPLOYEE_ID) AS CNT, 
            ROUND(AVG(E.SALARY), 2) AS AVG_S, 
            MAX(E.SALARY) AS MAX_S
        FROM 
            EMPLOYEES E
            JOIN DEPARTMENTS D ON E.DEPARTMENT_ID = D.DEPARTMENT_ID
            JOIN LOCATIONS L   ON D.LOCATION_ID = L.LOCATION_ID
        GROUP BY 
            L.LOCATION_ID, L.CITY
        ORDER BY 
            CNT ASC, 
            AVG_S DESC
    )
    LOOP
        -- Increment the rank variable for each row
        V_RANK := V_RANK + 1;

        -- Identify the job title for the highest paid employee in THIS city
        -- Added ROWNUM = 1 to handle cases where two people have the same max salary
        SELECT J.JOB_TITLE INTO V_JOB_TITLE
        FROM EMPLOYEES E
        JOIN JOBS J ON E.JOB_ID = J.JOB_ID
        JOIN DEPARTMENTS D ON E.DEPARTMENT_ID = D.DEPARTMENT_ID
        WHERE D.LOCATION_ID = X.LOCATION_ID
          AND E.SALARY = X.MAX_S
          AND ROWNUM = 1;

        -- Output the details
        DBMS_OUTPUT.PUT_LINE(
            RPAD(V_RANK, 6) || 
            RPAD(X.CITY, 15) || 
            RPAD(X.CNT, 8) || 
            RPAD(X.AVG_S, 12) || 
            V_JOB_TITLE
        );
        
    END LOOP;

EXCEPTION
    WHEN NO_DATA_FOUND THEN
        DBMS_OUTPUT.PUT_LINE('No data found for the report.');
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Error: ' || SQLERRM);
END;
/

-- Question 3. Transfer
-- Create a trigger that activates when an employee is transferred to a new department (i.e., when an UPDATE
-- operation on the department_id is performed in the Employee table).
-- 
-- 1 new table: Transfers
-- (Fields:
-- employee_id,
-- employee_working_instead_of_him,
-- new_department,
-- current date
-- )
-- 
-- Conditions:
--     1. If that employee had a manager,
--         a. An employee with the closest salary to him/her under the same manager will work instead of
--         him/her. The new salary for that work-in-place employee will be equal to previous salary of
--         that work-in-place employee + 0.5* salary of the transferred employee.
--         
--         b. His/her new manager in the new department should be the manager with the closest number of
--         subordinates to his/her previous manager.
-- 
-- No changes in the Job table and the Job_history table are necessary for your ease.

CREATE TABLE TRANSFERS (
    EMPLOYEE_ID NUMBER PRIMARY KEY, 
    SUB_EMPLOYEE_ID NUMBER, 
    NEW_DEPT_ID NUMBER,
    CURR_DATE DATE
);

CREATE OR REPLACE TRIGGER EMPLOYEE_TRANSFER
BEFORE UPDATE OF DEPARTMENT_ID ON EMPLOYEES
FOR EACH ROW
DECLARE
    -- Allows querying/updating the table without Mutating Table errors
    PRAGMA AUTONOMOUS_TRANSACTION; 

    V_SUB_EID        NUMBER;
    V_NEW_MANAGER_ID NUMBER;
    V_PREV_MAN_SUBS  NUMBER;
BEGIN
    -- Condition 1: Check if the employee had a manager
    IF :OLD.MANAGER_ID IS NOT NULL THEN
        
        -- 1a. Find the employee with the closest salary under the same manager
        BEGIN
            SELECT EMPLOYEE_ID INTO V_SUB_EID
            FROM (
                SELECT EMPLOYEE_ID
                FROM EMPLOYEES
                WHERE MANAGER_ID = :OLD.MANAGER_ID
                  AND EMPLOYEE_ID <> :OLD.EMPLOYEE_ID
                ORDER BY ABS(SALARY - :OLD.SALARY) ASC, EMPLOYEE_ID ASC
            )
            WHERE ROWNUM = 1;

            -- Update the salary of the substitute employee
            UPDATE EMPLOYEES
            SET SALARY = SALARY + (0.5 * :OLD.SALARY)
            WHERE EMPLOYEE_ID = V_SUB_EID;
        EXCEPTION
            WHEN NO_DATA_FOUND THEN V_SUB_EID := NULL;
        END;

        -- 1b. Find the new manager for the transferred employee
        -- Get subordinate count of previous manager
        SELECT COUNT(*) INTO V_PREV_MAN_SUBS
        FROM EMPLOYEES
        WHERE MANAGER_ID = :OLD.MANAGER_ID;

        -- Find manager with closest subordinate count (Condition 1.b)
        SELECT M_ID INTO V_NEW_MANAGER_ID
        FROM (
            SELECT MANAGER_ID AS M_ID
            FROM EMPLOYEES
            WHERE MANAGER_ID IS NOT NULL
              AND MANAGER_ID <> :OLD.MANAGER_ID -- Looking for a DIFFERENT manager
            GROUP BY MANAGER_ID
            ORDER BY ABS(COUNT(EMPLOYEE_ID) - V_PREV_MAN_SUBS) ASC, MANAGER_ID ASC
        )
        WHERE ROWNUM = 1;

        -- Assign the new manager to the transferred employee
        :NEW.MANAGER_ID := V_NEW_MANAGER_ID;

        -- Insert record into Transfers table
        INSERT INTO TRANSFERS (EMPLOYEE_ID, SUB_EMPLOYEE_ID, NEW_DEPT_ID, CURR_DATE)
        VALUES (:OLD.EMPLOYEE_ID, V_SUB_EID, :NEW.DEPARTMENT_ID, SYSDATE);

    END IF;

EXCEPTION
    WHEN OTHERS THEN
        -- Rollback the autonomous transaction on error
        ROLLBACK;
        DBMS_OUTPUT.PUT_LINE('Error in Transfer Trigger: ' || SQLERRM);
END;
/

--22 C
-- Question 1.
-- Write a PL/SQL function named IS_READY_FOR_PROMOTION that takes an
-- EMPLOYEE_ID as input and returns a VARCHAR2 result indicating whether the employee is
-- ready for promotion.
-- The function should return:
--   • YES if the employee meets all the following criteria:
--   o Has worked for at least 5 years since the HIRE_DATE
--   o Has a SALARY greater than the midpoint of their job’s MIN_SALARY and
--   MAX_SALARY
--   o Manages at least one subordinate.
--   • NO if the employee exists but fails any of the above conditions
-- For demonstration, you need to call the function IS_READY_FOR_PROMOTION for each
-- employee and output whether they are eligible or not. Make sure to handle exceptions with
-- appropriate messages.

CREATE OR REPLACE FUNCTION IS_READY_FOR_PROMOTION (IN_EMPLOYEE_ID IN NUMBER) 
RETURN VARCHAR2 IS 
    -- declare req'd vars here
    V_YEARS_WORKED NUMBER;
    V_SAL NUMBER;
    V_SAL_MIDPOINT NUMBER;
    V_SUB_COUNT NUMBER;
    MSG VARCHAR2(100);
BEGIN
    SELECT ROUND(MONTHS_BETWEEN(SYSDATE, HIRE_DATE) / 12) INTO V_YEARS_WORKED
    FROM EMPLOYEES
    WHERE EMPLOYEE_ID = IN_EMPLOYEE_ID;
    
    SELECT SALARY INTO V_SAL
    FROM EMPLOYEES 
    WHERE EMPLOYEE_ID = IN_EMPLOYEE_ID;
    
    SELECT (ABS(MAX_SALARY + MIN_SALARY) / 2) INTO V_SAL_MIDPOINT
    FROM JOBS 
    WHERE JOB_ID = (SELECT JOB_ID FROM EMPLOYEES WHERE EMPLOYEE_ID = IN_EMPLOYEE_ID);

    SELECT COUNT(*) INTO V_SUB_COUNT
    FROM EMPLOYEES
    WHERE MANAGER_ID = IN_EMPLOYEE_ID;
    
    IF V_YEARS_WORKED >= 5 AND V_SAL > V_SAL_MIDPOINT AND V_SUB_COUNT > 0 THEN
        MSG := 'YES';
    ELSE 
      MSG := 'NO';
    END IF;
  
    RETURN MSG;

EXCEPTION
  WHEN NO_DATA_FOUND THEN
      RETURN 'ID NOT FOUND';
  WHEN TOO_MANY_ROWS THEN
      RETURN 'TOO MANY ROWS';
  WHEN OTHERS THEN
      RETURN 'ERROR';

END;
/

DECLARE
BEGIN
    FOR EID IN (SELECT EMPLOYEE_ID FROM MY_EMPLOYEES ORDER BY EMPLOYEE_ID;)
    LOOP
        EXIT WHEN CUR%NOTFOUND;
        DBMS_OUTPUT.PUT_LINE('IS '|| TO_CHAR(EID) || ' READY FOR PROMOTION? ' || IS_READY_FOR_PROMOTION(EID));
    END LOOP;
END;
/


-- Question 2.
-- First create a copy of the EMPLOYEES table in the HR schema, named EMPLOYEES_COPY. Do all
-- the work in EMPLOYEES_COPY table.
-- Write a PL/SQL procedure that updates the salaries of all employees. The new salary for an employee
-- will be calculated as follows:
-- New Salary = Old Salary + (Commission Percentage * Old Salary) + 0.1 *
-- (Min Salary of Employee's Job) + 0.1 * (Average Salary of Employee's
-- Department)
-- Conditions:
-- 1. If the employee's work period is 1 year or less, his/her salary will not be updated. (Use the
-- hire date of the newest employee as today's date instead of SYSDATE).
-- 2. If the employee’s new salary exceeds the maximum salary for his/her job, set the new salary
-- to the maximum salary for that job.
-- 
-- The procedure should loop through all employees to update the salary. Handle appropriate exceptions.

-- To copy a table, execute the following script-
CREATE TABLE employees_copy AS
SELECT * FROM employees;
COMMIT;

CREATE OR REPLACE PROCEDURE UPDATE_SAL IS
    --DECLARE VARS 
    V_DATE DATE;
    V_HIRE_DATE DATE;
    V_COMM_PCT NUMBER;
    V_OLD_SAL NUMBER;
    V_MIN_SAL NUMBER;
    V_AVG_SAL NUMBER;
    V_NEW_SAL NUMBER;
    V_MAX_SAL NUMBER;
    V_EID NUMBER;
    
BEGIN
    FOR E IN (SELECT EMPLOYEE_ID FROM EMPLOYEES ORDER BY EMPLOYEE_ID)
    LOOP
        EID = E.EMPLOYEE_ID;
        
        SELECT MAX(HIRE_DATE) INTO V_DATE FROM EMPLOYEES;
        SELECT HIRE_DATE INTO V_HIRE_DATE FROM EMPLOYEES WHERE EID = EMPLOYEE_ID;
        
        IF MONTHS_BETWEEN(V_DATE, V_HIRE_DATE) >= 12 THEN
            SELECT NVL(COMMISSION_PCT, 0), SALARY INTO V_COMM_PCT, V_OLD_SAL
            FROM EMPLOYEES
            WHERE EMPLOYEE_ID = EID;
            
            SELECT AVG(SALARY) INTO V_AVG_SAL
            FROM EMPLOYEES
            WHERE DEPARTMENT_ID = (SELECT DEPARTMENT_ID FROM EMPLOYEES WHERE EID = EMPLOYEE_ID);
            
            SELECT MIN_SALARY, MAX_SALARY INTO V_MIN_SAL, V_MAX_SAL
            FROM JOBS
            WHERE JOB_ID = (SELECT JOB_ID FROM EMPLOYEES WHERE EID = EMPLOYEE_ID);
            
            V_NEW_SAL := (V_OLD_SAL) + 0.1 * V_MIN_SAL + V_COMM_PCT * V_OLD_SAL + 0.1 * V_AVG_SAL;
            
            IF V_NEW_SAL > V_MAX_SAL THEN
                V_NEW_SAL := V_MAX_SAL;
            END IF;
            
            UPDATE EMPLOYEES
            SET SALARY = V_NEW_SAL
            WHERE EMPLOYEE_ID = EID;
        END IF;     
    END LOOP;
    
EXCEPTION 
  WHEN NO_DATA_FOUND THEN
      DBMS_OUTPUT.PUT_LINE('ALKDJ');
  WHEN TOO_MANY_ROWS THEN
      DBMS_OUTPUT.PUT_LINE('ALKDJ');
  WHEN OTHERS THEN
      DBMS_OUTPUT.PUT_LINE('ALKDJ');

END;
/

-- Question 3.
-- Create a trigger that activates when an employee’s salary is updated.
-- Salary decrease>20% → demotion
-- 1 new table: Demotions
-- (fields:
-- employee_id,
-- current_salary,
-- Status- waiting and done,
-- date
-- )
-- In case of demotion:
-- 1. No change if that employee is not a manager
-- 2. If that employee is a manager, switch him/her with the highest-paid employee under him/her. (Do
-- not switch salaries)
-- No changes in the Job table and the Job_history table are necessary for your ease. All current date is
-- the hire date of the newest employee instead of SYSDATE.
CREATE TABLE DEMOTIONS (
    EMPLOYEE_ID NUMBER PRIMARY KEY,
    CURRENT_SALARY NUMBER,
    STATUS VARCHAR2(50),
    DEMOTION_DATE DATE
);

CREATE OR REPLACE TRIGGER trg_salary_demotion
BEFORE UPDATE OF salary ON employees
FOR EACH ROW
WHEN (NEW.salary < OLD.salary * 0.8) -- Fires ONLY if salary drops more than 20%
DECLARE
    -- This allows us to query the employees table without the Mutating Table error
    PRAGMA AUTONOMOUS_TRANSACTION; 
    
    v_demotion_date DATE;
    v_sub_count     NUMBER;
    v_highest_sub   NUMBER;
BEGIN
    -- 1. Find the "current date" (hire date of the newest employee)
    SELECT MAX(hire_date) INTO v_demotion_date FROM employees;

    -- 2. Log the initial demotion as 'waiting'
    INSERT INTO Demotions (employee_id, current_salary, status, demotion_date)
    VALUES (:NEW.employee_id, :NEW.salary, 'waiting', v_demotion_date);

    -- 3. Check if the demoted employee is a manager (do they have subordinates?)
    SELECT COUNT(*) INTO v_sub_count
    FROM employees
    WHERE manager_id = :OLD.employee_id;

    -- 4. Manager Logic
    IF v_sub_count > 0 THEN
        -- Find the highest-paid subordinate
        SELECT employee_id INTO v_highest_sub
        FROM (
            SELECT employee_id
            FROM employees
            WHERE manager_id = :OLD.employee_id
            ORDER BY salary DESC, employee_id ASC
        )
        WHERE ROWNUM = 1;

        -- THE SWITCH
        -- A. Move all other subordinates to report to the new manager
        UPDATE employees
        SET manager_id = v_highest_sub
        WHERE manager_id = :OLD.employee_id
          AND employee_id != v_highest_sub;
          
        -- B. The new manager moves up to report to the old manager's boss
        UPDATE employees
        SET manager_id = :OLD.manager_id
        WHERE employee_id = v_highest_sub;

        -- C. The demoted manager now reports to the new manager.
        -- *CRITICAL*: We use :NEW to change this instantly. Doing an actual 
        -- UPDATE employees SET... here would crash the database with a row lock!
        :NEW.manager_id := v_highest_sub;
    END IF;

    -- 5. Finalize the status to 'done' (applies to both managers and regular employees)
    UPDATE Demotions
    SET status = 'done'
    WHERE employee_id = :NEW.employee_id 
      AND status = 'waiting';

    -- 6. Autonomous transactions must always be committed
    COMMIT;
END;
/


-- Create the following table.
-- 
-- CREATE TABLE EMP_SAL_LOG (
-- LOG_ID NUMBER,
-- EMPLOYEE_ID NUMBER,
-- OLD_SALARY NUMBER,
-- NEW_SALARY NUMBER,
-- MOD_DATE DATE,
-- USER_ID NUMBER,
-- USERNAME VARCHAR2(100),
-- STATUS VARCHAR2(10)
-- );
-- 1. Write a PL/SQL trigger that will fire only when one or more employees have been added to or removed from
-- the EMPLOYEES table. The trigger should populate the EMP_SAL_LOG table with corresponding values. The
-- STATUS should be ‘APPROVED’. MOD_DATE is the current date. LOG_ID should be incremented
-- sequentially (starting from 1). Keep the irrelevant fields null. You may consider that EMPLOYEE_ID will
-- always be unique during insertion into the EMPLOYEES table.
CREATE SEQUENCE emp_sal_log_seq START WITH 1 INCREMENT BY 1;

CREATE OR REPLACE TRIGGER trg_emp_add_remove
AFTER INSERT OR DELETE ON employees
FOR EACH ROW
BEGIN
    IF INSERTING THEN
        INSERT INTO EMP_SAL_LOG (LOG_ID, EMPLOYEE_ID, OLD_SALARY, NEW_SALARY, MOD_DATE, STATUS)
        VALUES (emp_sal_log_seq.NEXTVAL, :NEW.employee_id, NULL, :NEW.salary, SYSDATE, 'APPROVED');
    ELSIF DELETING THEN
        INSERT INTO EMP_SAL_LOG (LOG_ID, EMPLOYEE_ID, OLD_SALARY, NEW_SALARY, MOD_DATE, STATUS)
        VALUES (emp_sal_log_seq.NEXTVAL, :OLD.employee_id, :OLD.salary, NULL, SYSDATE, 'APPROVED');
    END IF;
END;
/





-- 2. Write another PL/SQL trigger that will fire only when the salary of an existing employee is updated. The
-- trigger should populate the EMP_SAL_LOG table with corresponding values. You must ensure that salary is not
-- changed in either of the following cases.
-- a. If the last update was less than one month ago.
-- b. If the increment or decrement is more than 20%.
-- In any of these cases, users should be informed like ‘SALARY UPDATE FAILED’ and STATUS should be
-- ‘DENIED’. Otherwise STATUS should be ‘APPROVED’. MOD_DATE is the current date. LOG_ID should
-- be incremented sequentially.

CREATE OR REPLACE TRIGGER UP_SAL_TRIG
BEFORE UPDATE OF SALARY ON EMPLOYEES
FOR EACH ROW
DECLARE
    V_LAST_UPDATE DATE;

BEGIN
    BEGIN
        SELECT MOD_DATE INTO V_LAST_UPDATE
        FROM ( 
            SELECT MOD_DATE
            FROM EMP_SAL_LOG 
            WHERE EMPLOYEE_ID = :OLD.EMPLOYEE_ID AND OLD_SALARY IS NOT NULL AND NEW_SALARY IS NOT NULL
            ORDER BY MOD_DATE DESC
        )
        WHERE ROWNUM = 1;
    EXCEPTION
        WHEN NO_DATA_FOUND THEN
            V_LAST_UPDATE := TO_DATE('01-01-2000');
    END;
    
    IF MONTHS_BETWEEN(SYSDATE, V_LAST_UPDATE) > 1 AND (:NEW.SALARY < 1.2 * :OLD.SALARY OR :NEW.SALARY > 0.8 * :OLD.SALARY) THEN
        INSERT INTO EMP_SAL_LOG (LOG_ID, EMPLOYEE_ID, OLD_SALARY, NEW_SALARY, MOD_DATE, STATUS)
        VALUES (emp_sal_log_seq.NEXTVAL, :NEW.employee_id, :OLD.SALARY, :NEW.salary, SYSDATE, 'APPROVED');
    ELSE 
        :NEW_SALARY := :OLD.SALARY;
        DBMS_OUTPUT.PUT_LINE('SALARY UPDATE FAILED');
        INSERT INTO EMP_SAL_LOG (LOG_ID, EMPLOYEE_ID, OLD_SALARY, NEW_SALARY, MOD_DATE, STATUS)
        VALUES (emp_sal_log_seq.NEXTVAL, :NEW.employee_id, :OLD.SALARY, :NEW.salary, SYSDATE, 'FAILED');
        
END;
/


-- 1. Write a PL/SQL trigger that will enforce the following business rule:
-- The total salary paid to all employees in any single department must never exceed
-- $50,000 after any salary modification.
CREATE OR REPLACE TRIGGER trg_limit_dept_salary
AFTER INSERT OR UPDATE OF salary, department_id ON EMPLOYEES
FOR EACH ROW
DECLARE
    v_total_salary NUMBER;
BEGIN
    -- Calculate the new total for the affected department
    SELECT SUM(salary)
    INTO v_total_salary
    FROM EMPLOYEES
    WHERE department_id = :NEW.department_id;

    -- Enforce the $50,000 limit
    IF v_total_salary > 50000 THEN
        RAISE_APPLICATION_ERROR(-20001, 'Transaction rejected: Department total salary exceeds $50,000.');
    END IF;
END;
/



-- 2. Write a PL/SQL function that checks whether an employee is currently paid more than
-- they should be according to their job and years of service.
-- The function should:
-- ● Take one input parameter: employee ID
-- ● Check if the employee exists in the EMPLOYEES table. If not → return NULL
-- ● Calculate years of service = number of full years since hire date until today (use
-- FLOOR)
-- ● Apply this simple overpay rule:
--     ○ If years of service ≥ 10 → employee should not earn more than 95% of
--     maximum salary
--     ○ If years of service ≥ 5 and < 10 → employee should not earn more than
--     90% of the maximum salary
--     ○ If years of service < 5 → employee should not earn more than 85% of the
--     maximum salary
--     ● Return:
--     ○ 1 → if the employee is overpaid (salary > the allowed percentage of
--     maximum salary)
--     ○ 0 → if the employee is within the allowed range
--     ○ -1 → if the employee has no job or the job data is missing 

CREATE OR REPLACE FUNCTION check_overpaid(p_emp_id IN NUMBER) 
RETURN NUMBER 
IS
    v_salary      EMPLOYEES.salary%TYPE;
    v_hire_date   EMPLOYEES.hire_date%TYPE;
    v_max_salary  JOBS.max_salary%TYPE;
    v_years       NUMBER;
    v_limit_pct   NUMBER;
BEGIN
    -- 1. Check if employee exists and fetch data
    SELECT salary, hire_date, job_id 
    INTO v_salary, v_hire_date, v_max_salary
    FROM EMPLOYEES e
    JOIN JOBS j ON e.job_id = j.job_id
    WHERE employee_id = p_emp_id;

    -- 2. Calculate years of service
    v_years := FLOOR(MONTHS_BETWEEN(SYSDATE, v_hire_date) / 12);

    -- 3. Determine the percentage limit based on years
    IF v_years >= 10 THEN
        v_limit_pct := 0.95;
    ELSIF v_years >= 5 THEN
        v_limit_pct := 0.90;
    ELSE
        v_limit_pct := 0.85;
    END IF;

    -- 4. Check for overpayment
    IF v_salary > (v_max_salary * v_limit_pct) THEN
        RETURN 1;  -- Overpaid
    ELSE
        RETURN 0;  -- Within range
    END IF;

EXCEPTION
    WHEN NO_DATA_FOUND THEN
        -- Check if it's the employee missing or just the job data
        RETURN NULL; 
    WHEN OTHERS THEN
        RETURN -1; -- General error/missing job data
END;
/


