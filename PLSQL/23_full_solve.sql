--------------------------------------------------------------------------------
-- =============================================================================
-- BASIC SQL PROBLEMS
-- =============================================================================
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
-- MAKEUP ONLINE A1 & B1/B2 (Shared Basic Questions)
--------------------------------------------------------------------------------

-- Q: Show the employee ID, salary, and salary_diff, where salary_diff is the absolute
-- difference between the salary and the nearest multiple of 1,000.
SELECT employee_id, 
       salary, 
       ABS(salary - ROUND(salary, -3)) AS salary_diff
FROM employees;

-- Q: Create a table named EMP_TRAINING with columns: TRAINING_ID, EMPLOYEE_ID, 
-- TRAINING_NAME, REG_DATE. Link to employee, name must be provided, reg_date 
-- defaults to sysdate. Insert one record.
CREATE TABLE EMP_TRAINING (
    TRAINING_ID NUMBER PRIMARY KEY,
    EMPLOYEE_ID NUMBER REFERENCES EMPLOYEES(EMPLOYEE_ID),
    TRAINING_NAME VARCHAR2(100) NOT NULL,
    REG_DATE DATE DEFAULT SYSDATE
);

INSERT INTO EMP_TRAINING (TRAINING_ID, EMPLOYEE_ID, TRAINING_NAME) 
VALUES (1, 100, 'Oracle SQL Fundamentals');

-- Q: Display the Manager ID and the "Total Monthly Liability" (sum of salaries) for each manager.
-- Only display managers whose Liability > 30,000 and who manage >= 5 people. Order desc.
SELECT manager_id, 
       SUM(salary) AS "Total Monthly Liability"
FROM employees
WHERE manager_id IS NOT NULL
GROUP BY manager_id
HAVING SUM(salary) > 30000 
   AND COUNT(employee_id) >= 5
ORDER BY "Total Monthly Liability" DESC;

-- Q: Find Emp ID, Hire Date, and Commission Pct for winter months (Dec, Jan, Feb).
-- Display null commission as "No Commission". Salary > 8000. Order by Month of hire (Dec, Jan, Feb).
SELECT employee_id, 
       hire_date,
       NVL(TO_CHAR(commission_pct), 'No Commission') AS commission_pct
FROM employees
WHERE TO_CHAR(hire_date, 'MM') IN ('12', '01', '02')
  AND salary > 8000
ORDER BY CASE TO_CHAR(hire_date, 'MM') 
            WHEN '12' THEN 1 
            WHEN '01' THEN 2 
            WHEN '02' THEN 3 
         END;

-- Q: Display Job ID and "Salary Gap" (Max - Min) for jobs where gap > 3000.
-- Exclude jobs where avg salary < 6000. Sort by Salary Gap desc.
SELECT job_id, 
       (MAX(salary) - MIN(salary)) AS "Salary Gap"
FROM employees
GROUP BY job_id
HAVING (MAX(salary) - MIN(salary)) > 3000
   AND AVG(salary) >= 6000
ORDER BY "Salary Gap" DESC;


--------------------------------------------------------------------------------
-- BASIC SQL ASSIGNMENT - A1/A2
--------------------------------------------------------------------------------

-- Q: Group employees by hiring month (JAN, FEB..). For each month, display the number of employees 
-- whose last name contains "a" (case-insensitive). Sort chronologically.
SELECT TO_CHAR(hire_date, 'MON') AS hiring_month, 
       COUNT(employee_id) AS num_employees
FROM employees
WHERE LOWER(last_name) LIKE '%a%'
GROUP BY TO_CHAR(hire_date, 'MON'), TO_CHAR(hire_date, 'MM')
ORDER BY TO_CHAR(hire_date, 'MM');

-- Q: Show the employee ID and last name of employees whose last name contains at least one pair 
-- of identical consecutive characters (case-insensitive). Sort by last name.
SELECT employee_id, 
       last_name
FROM employees
WHERE last_name like '%aa%' 
    or last_name like '%bb%'...
ORDER BY last_name;

-- Q: Create PERFORMANCE_REVIEW table: REVIEW_ID, EMPLOYEE_ID, REVIEW_YEAR, RATING, REMARKS.
-- Rating between 1 and 5. Remarks optional. Link to emp. Insert one record.
CREATE TABLE PERFORMANCE_REVIEW (
    REVIEW_ID NUMBER PRIMARY KEY,
    EMPLOYEE_ID NUMBER REFERENCES EMPLOYEES(EMPLOYEE_ID),
    REVIEW_YEAR NUMBER,
    RATING NUMBER NOT NULL CHECK (RATING BETWEEN 1 AND 5),
    REMARKS VARCHAR2(500)
);

INSERT INTO PERFORMANCE_REVIEW (REVIEW_ID, EMPLOYEE_ID, REVIEW_YEAR, RATING, REMARKS) 
VALUES (1, 100, 2023, 5, 'Excellent performance this year.');

-- Q: Find Emp ID, full name, and "User ID" (first 3 letters of first name + last 3 digits of phone).
-- Phone length >= 10, dept 50 or 80. Sort by first name length desc.
SELECT employee_id,
       first_name || ' ' || last_name AS full_name,
       SUBSTR(first_name, 1, 3) || SUBSTR(phone_number, -3) AS "User ID"
FROM employees
WHERE LENGTH(phone_number) >= 10
  AND department_id IN (50, 80)
ORDER BY LENGTH(first_name) DESC;


--------------------------------------------------------------------------------
-- BASIC SQL ASSIGNMENT - C1/C2
--------------------------------------------------------------------------------

-- Q: Group employees by job acronym (Y in X_Y format). For each group, print the number 
-- of employees who earn > 4000 and do not receive any commission.
SELECT SUBSTR(job_id, INSTR(job_id, '_') + 1) AS job_acronym,
       COUNT(employee_id) AS num_employees
FROM employees
WHERE salary > 4000 
  AND commission_pct IS NULL
GROUP BY SUBSTR(job_id, INSTR(job_id, '_') + 1);


-- Q: Show Emp ID, first name, last name, and TAG (1st letter first name + 1st letter last name).
-- Show only if these two letters are the same. Sort by employee ID.
SELECT employee_id, 
       first_name, 
       last_name,
       UPPER(SUBSTR(first_name, 1, 1) || SUBSTR(last_name, 1, 1)) AS TAG
FROM employees
WHERE UPPER(SUBSTR(first_name, 1, 1)) = UPPER(SUBSTR(last_name, 1, 1))
ORDER BY employee_id;

-- Q: Create EMP_BONUS: BONUS_ID, EMPLOYEE_ID, BONUS_AMOUNT. 
-- Link to emp, amount provided, cannot be negative. Insert one record.
CREATE TABLE EMP_BONUS (
    BONUS_ID NUMBER PRIMARY KEY,
    EMPLOYEE_ID NUMBER REFERENCES EMPLOYEES(EMPLOYEE_ID),
    BONUS_AMOUNT NUMBER NOT NULL CHECK (BONUS_AMOUNT >= 0)
);

INSERT INTO EMP_BONUS (BONUS_ID, EMPLOYEE_ID, BONUS_AMOUNT) 
VALUES (1, 100, 1500);


--------------------------------------------------------------------------------
-- BASIC SQL ASSIGNMENT - B1/B2 (Uniques)
--------------------------------------------------------------------------------

-- Q: Group employees by country code (first part of phone before dot). 
-- Count employees joined after 1st January 2005.
SELECT SUBSTR(phone_number, 1, INSTR(phone_number, '.') - 1) AS country_code,
       COUNT(employee_id) AS num_employees
FROM employees
WHERE hire_date > TO_DATE('01-JAN-2005', 'DD-MON-YYYY')
GROUP BY SUBSTR(phone_number, 1, INSTR(phone_number, '.') - 1);

-- Q: Show Dept ID and total annual payout (sum of monthly * 12).
-- Only include depts where emps > 2 and min salary >= 2500. Format '99,999,999'.
SELECT department_id,
       TO_CHAR(SUM(salary * 12), '99,999,999') AS total_annual_payout
FROM employees
WHERE department_id IS NOT NULL
GROUP BY department_id
HAVING COUNT(employee_id) > 2 
   AND MIN(salary) >= 2500;

-- Q: Display Emp ID and "Security Code" (upper first 2 letters first name + length last name + last digit phone).
-- Only show if length first name > length last name. Order by length Security Code desc.
SELECT employee_id,
       UPPER(SUBSTR(first_name, 1, 2)) || LENGTH(last_name) || SUBSTR(phone_number, -1) AS "Security Code"
FROM employees
WHERE LENGTH(first_name) > LENGTH(last_name)
ORDER BY LENGTH(UPPER(SUBSTR(first_name, 1, 2)) || LENGTH(last_name) || SUBSTR(phone_number, -1)) DESC;



-- Q1: List managers whose departments have average salaries higher than the overall company average, 
-- for departments located in Toronto and Oxford.
SELECT m.employee_id, 
       m.first_name, 
       m.last_name
FROM employees m
JOIN departments d ON m.employee_id = d.manager_id
JOIN locations l ON d.location_id = l.location_id
WHERE l.city IN ('Toronto', 'Oxford')
  AND d.department_id IN (
      SELECT department_id 
      FROM employees 
      GROUP BY department_id 
      HAVING AVG(salary) > (SELECT AVG(salary) FROM employees)
  );


-- Q2: Find employees who both work in departments with more than 5 employees AND have salaries 
-- greater than the overall average salary across all employees.
SELECT employee_id, 
       first_name, 
       last_name, 
       salary
FROM employees e1
WHERE salary > (SELECT AVG(salary) FROM employees)
  AND 5 <= (SELECT count(*) FROM employees e2 WHERE e2.department_id = e1.department_id); 


-- Q3: Write a SQL query for employees in departments that have managers, with no job history records, 
-- and salary > dept average. Show full_name, salary, dept_name, and label 'Stable High Earner' if 
-- salary > 1.7 times dept average, else 'Dept Above Avg'.
SELECT e.first_name || ' ' || e.last_name AS full_name, 
       e.salary, 
       d.department_name,
       CASE 
           WHEN e.salary > 1.7 * (SELECT AVG(salary) FROM employees WHERE department_id = e.department_id) 
           THEN 'Stable High Earner' 
           ELSE 'Dept Above Avg' 
       END AS Label
FROM employees e
JOIN departments d ON e.department_id = d.department_id
WHERE d.manager_id IS NOT NULL
  AND NOT EXISTS (SELECT 1 FROM job_history jh WHERE jh.employee_id = e.employee_id)
  AND e.salary > (SELECT AVG(salary) FROM employees WHERE department_id = e.department_id);


-- Q4: Find employees who are either in departments with more than 5 employees or have a job with 
-- minimum salary above 10000. 
-- Display: employee_id, first_name, last_name, department_id, job_id, salary.
SELECT e.employee_id, 
       e.first_name, 
       e.last_name, 
       e.department_id, 
       e.job_id, 
       e.salary
FROM employees e
JOIN jobs j ON e.job_id = j.job_id
WHERE e.department_id IN (
      SELECT department_id 
      FROM employees 
      GROUP BY department_id 
      HAVING COUNT(*) > 5
  )
   OR j.min_salary > 10000;


-- Q5: Write an SQL query to find employees who satisfy exactly one of the following conditions: 
-- (i) they work in a department with more than 5 employees, or 
-- (ii) their job has a minimum salary greater than 10000. 
-- Employees who satisfy both conditions or neither condition must be excluded. Display employee 
-- ID, full name, department ID, job ID, and salary.
SELECT e.employee_id, 
       e.first_name || ' ' || e.last_name AS full_name, 
       e.department_id, 
       e.job_id, 
       e.salary
FROM employees e
JOIN jobs j ON e.job_id = j.job_id
WHERE (CASE WHEN e.department_id IN (SELECT department_id FROM employees GROUP BY department_id HAVING COUNT(*) > 5) THEN 1 ELSE 0 END)
    + (CASE WHEN j.min_salary > 10000 THEN 1 ELSE 0 END) = 1;
-- xor = ab' + a'b


-- Q1: Find employees earning above their department's average salary in departments with more than 4 
-- employees.
SELECT employee_id, 
       first_name, 
       last_name, 
       salary, 
       department_id
FROM employees e
WHERE salary > (
      SELECT AVG(salary) 
      FROM employees 
      WHERE department_id = e.department_id
  )
  AND department_id IN (
      SELECT department_id 
      FROM employees 
      GROUP BY department_id 
      HAVING COUNT(*) > 4
  );


-- Q2: Find employees who either earn more than their manager's salary or have a salary greater than 
-- their department's average salary. Print employee details with the type as either "Higher Than 
-- Manager" or "Above Dept Avg".
SELECT e.employee_id, 
       e.first_name, 
       e.last_name, 
       e.salary,
       CASE 
           WHEN e.salary > m.salary THEN 'Higher Than Manager'
           WHEN e.salary > (SELECT AVG(salary) FROM employees WHERE department_id = e.department_id) THEN 'Above Dept Avg'
       END AS Type
FROM employees e
JOIN employees m ON e.manager_id = m.employee_id
WHERE e.salary > m.salary
   OR e.salary > (SELECT AVG(salary) FROM employees WHERE department_id = e.department_id);


-- Q3: Write a SQL query for employees whose salary beats their department average and whose 
-- manager's salary beats the company average. Show full_name, salary, department_name, and 
-- label it 'Dept Top Earner' if salary > 1.5 times dept average, else 'Dept Above Avg'.
SELECT e.first_name || ' ' || e.last_name AS full_name, 
       e.salary, 
       d.department_name,
       CASE 
           WHEN e.salary > 1.5 * (SELECT AVG(salary) FROM employees WHERE department_id = e.department_id) THEN 'Dept Top Earner' 
           ELSE 'Dept Above Avg' 
       END AS Label
FROM employees e
JOIN departments d ON e.department_id = d.department_id
JOIN employees m ON e.manager_id = m.employee_id
WHERE e.salary > (SELECT AVG(salary) FROM employees WHERE department_id = e.department_id)
  AND m.salary > (SELECT AVG(salary) FROM employees);


-- Q4: Find employee_id, full name, and department name of employees whose department is located in 
-- the same city as their manager’s department.
SELECT e.employee_id, 
       e.first_name || ' ' || e.last_name AS full_name, 
       d.department_name
FROM employees e
JOIN departments d ON e.department_id = d.department_id
JOIN locations l_emp ON d.location_id = l_emp.location_id
JOIN employees m ON e.manager_id = m.employee_id
JOIN departments md ON m.department_id = md.department_id
JOIN locations l_mgr ON md.location_id = l_mgr.location_id
WHERE l_emp.city = l_mgr.city;


-- Q5: Write an SQL query to list all departments that satisfy the following conditions: 
-- (i) every employee in the department earns more than 5000, 
-- (ii) the department has at least one employee with job history, and 
-- (iii) the maximum salary in the department is greater than the overall company average salary. 
-- For each such department, display the department name, number of employees, average salary, and a 
-- column called Salary_Level that shows 
-- ● 'Elite' if the department’s average salary is greater than 1.5 times the company average salary, 
-- ● 'Above Average' otherwise.
SELECT d.department_name, 
       COUNT(e.employee_id) AS number_of_employees, 
       AVG(e.salary) AS average_salary,
       CASE 
           WHEN AVG(e.salary) > 1.5 * (SELECT AVG(salary) FROM employees) THEN 'Elite' 
           ELSE 'Above Average' 
       END AS Salary_Level
FROM departments d
JOIN employees e ON d.department_id = e.department_id
GROUP BY d.department_id, d.department_name
HAVING MIN(e.salary) > 5000
   AND d.department_id IN (SELECT DISTINCT department_id FROM job_history)
   AND MAX(e.salary) > (SELECT AVG(salary) FROM employees);


-- Q1: Find job titles in departments where employees have worked more than 5 years on average, but 
-- only include those jobs which have a maximum salary higher than the average max salary across 
-- all jobs.
SELECT j.job_title
FROM jobs j
WHERE j.job_id IN (
    SELECT e.job_id
    FROM employees e
    WHERE e.department_id IN (
        SELECT department_id
        FROM employees
        GROUP BY department_id
        HAVING AVG(MONTHS_BETWEEN(SYSDATE, hire_date) / 12) > 5
    )
)
AND j.max_salary > (SELECT AVG(max_salary) FROM jobs);


-- Q2: Find employees who earn more than their department's average salary but do NOT work in 
-- departments with more than 5 employees.
SELECT employee_id, 
       first_name, 
       last_name, 
       salary, 
       department_id
FROM employees e
WHERE salary > (
      SELECT AVG(salary) 
      FROM employees 
      WHERE department_id = e.department_id
  )
  AND department_id NOT IN (
      SELECT department_id 
      FROM employees 
      WHERE department_id IS NOT NULL 
      GROUP BY department_id 
      HAVING COUNT(*) > 5
  );


-- Q3: Write a SQL query to find employees from the USA who have a manager (using EXISTS), no 
-- job_history records (using NOT EXISTS), and salary greater than their department average. 
-- Display full_name, salary, and use a CASE statement to label 'USA Star' if salary > 1.4 times 
-- department average, otherwise 'USA Above'.
SELECT e.first_name || ' ' || e.last_name AS full_name, 
       e.salary,
       CASE 
           WHEN e.salary > 1.4 * (SELECT AVG(salary) FROM employees WHERE department_id = e.department_id) 
           THEN 'USA Star' 
           ELSE 'USA Above' 
       END AS USA_Label
FROM employees e
JOIN departments d ON e.department_id = d.department_id
JOIN locations l ON d.location_id = l.location_id
WHERE l.country_id = 'US'
  AND EXISTS (SELECT 1 FROM employees m WHERE m.employee_id = e.manager_id)
  AND NOT EXISTS (SELECT 1 FROM job_history jh WHERE jh.employee_id = e.employee_id)
  AND e.salary > (SELECT AVG(salary) FROM employees WHERE department_id = e.department_id);


-- Q4: Write an SQL query to list all departments where every employee earns more than 5000. For each 
-- department, display the department name, the number of employees in that department, and a 
-- column called Salary_Level that uses a CASE statement to show 'Above' if the department’s 
-- average salary is higher than the overall company average salary, or 'Below or Equal' if it is not.
SELECT d.department_name, 
       COUNT(e.employee_id) AS number_of_employees,
       CASE 
           WHEN AVG(e.salary) > (SELECT AVG(salary) FROM employees) THEN 'Above' 
           ELSE 'Below or Equal' 
       END AS Salary_Level
FROM departments d
JOIN employees e ON d.department_id = e.department_id
GROUP BY d.department_id, d.department_name
HAVING MIN(e.salary) > 5000;


-- Q5: Write an SQL query to find employees who earn more than the highest salary of at least one other 
-- department. Only include employees whose department has at least 3 employees and who do not 
-- have any records in the JOB_HISTORY table. For each qualifying employee, display the 
-- employee ID, full name, department name, and salary.
SELECT e.employee_id, 
       e.first_name || ' ' || e.last_name AS full_name, 
       d.department_name, 
       e.salary
FROM employees e
JOIN departments d ON e.department_id = d.department_id
WHERE e.salary > ANY (
      SELECT MAX(salary) 
      FROM employees 
      WHERE department_id IS NOT NULL 
        AND department_id != e.department_id
      GROUP BY department_id
  )
  AND e.department_id IN (
      SELECT department_id 
      FROM employees 
      GROUP BY department_id 
      HAVING COUNT(*) >= 3
  )
  AND NOT EXISTS (
      SELECT 1 FROM job_history jh WHERE jh.employee_id = e.employee_id
  );


-- =============================================================================
-- PL/SQL PROBLEMS
-- =============================================================================

-- Q1: Write a PL/SQL trigger that will enforce the following business rule: 
-- The total salary paid to all employees in any single department must never exceed 
-- $50,000 after any salary modification.
-- 
-- Note: We use a statement-level trigger (no "FOR EACH ROW") to check the 
-- total table after the update/insert is complete. This avoids the famous 
-- Oracle "Mutating Table" error (ORA-04091).

CREATE OR REPLACE TRIGGER trg_check_dept_total_salary
AFTER INSERT OR UPDATE OF salary, department_id ON employees
BEGIN
    -- Loop through any department that violates the $50,000 rule
    FOR dept_rec IN (
        SELECT department_id, SUM(salary) AS total_salary
        FROM employees
        WHERE department_id IS NOT NULL
        GROUP BY department_id
        HAVING SUM(salary) > 50000
    ) LOOP
        -- If the loop finds any department over 50k, it raises an error and halts the transaction
        RAISE_APPLICATION_ERROR(-20001, 'Salary modification failed! Department ' 
                                        || dept_rec.department_id 
                                        || ' total salary exceeds $50,000.');
    END LOOP;
END;
/


-- Q2: Write a PL/SQL function that checks whether an employee is currently paid more than 
-- they should be according to their job and years of service.
-- The function should: 
-- ● Take one input parameter: employee ID 
-- ● Check if the employee exists in the EMPLOYEES table. If not → return NULL 
-- ● Calculate years of service = number of full years since hire date until today (use FLOOR) 
-- ● Apply this simple overpay rule: 
--   ○ If years of service ≥ 10 → employee should not earn more than 95% of maximum salary 
--   ○ If years of service ≥ 5 and < 10 → employee should not earn more than 90% of the maximum salary 
--   ○ If years of service < 5 → employee should not earn more than 85% of the maximum salary 
-- ● Return: 
--   ○ 1 → if the employee is overpaid
--   ○ 0 → if the employee is within the allowed range 
--   ○ -1 → if the employee has no job or the job data is missing

CREATE OR REPLACE FUNCTION check_emp_overpaid (p_emp_id IN NUMBER) 
RETURN NUMBER IS
    v_salary      NUMBER;
    v_hire_date   DATE;
    v_job_id      VARCHAR2(20);
    v_max_salary  NUMBER;
    v_years       NUMBER;
    v_max_allowed NUMBER;
BEGIN
    -- Fetch employee and job info simultaneously using a LEFT JOIN
    -- If employee doesn't exist, this will instantly jump to the EXCEPTION block
    SELECT e.salary, e.hire_date, e.job_id, j.max_salary
    INTO v_salary, v_hire_date, v_job_id, v_max_salary
    FROM employees e
    LEFT JOIN jobs j ON e.job_id = j.job_id
    WHERE e.employee_id = p_emp_id;

    -- Return -1 if job data or max_salary is missing
    IF v_job_id IS NULL OR v_max_salary IS NULL THEN
        RETURN -1;
    END IF;

    -- Calculate full years of service using MONTHS_BETWEEN and FLOOR
    v_years := FLOOR(MONTHS_BETWEEN(SYSDATE, v_hire_date) / 12);

    -- Apply the overpay percentage rules to calculate maximum allowed salary
    IF v_years >= 10 THEN
        v_max_allowed := v_max_salary * 0.95;
    ELSIF v_years >= 5 AND v_years < 10 THEN
        v_max_allowed := v_max_salary * 0.90;
    ELSE
        v_max_allowed := v_max_salary * 0.85;
    END IF;

    -- Compare actual salary to allowed salary
    IF v_salary > v_max_allowed THEN
        RETURN 1;  -- Overpaid
    ELSE
        RETURN 0;  -- Within range
    END IF;

EXCEPTION
    WHEN NO_DATA_FOUND THEN
        -- If the SELECT statement finds no employee matching the ID
        RETURN NULL; 
END;
/


-- =============================================================================
-- PL/SQL PROBLEMS
-- =============================================================================

-- Q1: Write a PL/SQL trigger that will enforce the following business rule: 
-- No employee’s salary should ever exceed the maximum salary and fall below the 
-- minimum salary defined for their job in the JOBS table.

CREATE OR REPLACE TRIGGER trg_enforce_salary_bounds
BEFORE INSERT OR UPDATE OF salary, job_id ON employees
FOR EACH ROW
DECLARE
    v_min_salary jobs.min_salary%TYPE;
    v_max_salary jobs.max_salary%TYPE;
BEGIN
    -- Fetch the min and max salary limits for the employee's (new) job
    SELECT min_salary, max_salary 
    INTO v_min_salary, v_max_salary
    FROM jobs 
    WHERE job_id = :NEW.job_id;

    -- Check if the provided salary falls outside the allowed boundaries
    IF :NEW.salary < v_min_salary OR :NEW.salary > v_max_salary THEN
        -- Raise an error to halt the transaction and warn the user
        RAISE_APPLICATION_ERROR(-20002, 'Salary modification failed! The salary ' 
                                        || :NEW.salary 
                                        || ' is out of bounds for job ' 
                                        || :NEW.job_id 
                                        || '. Allowed range: ' || v_min_salary 
                                        || ' to ' || v_max_salary || '.');
    END IF;
END;
/


-- Q2: Write a PL/SQL procedure that does the following: 
-- ● Takes two input values: employee ID and new department ID 
-- ● If the employee exists and the department is valid: 
--   ○ Save the current job details into the JOB_HISTORY table before the change 
--   ○ Move the employee to the new department 
--   ○ Increase their salary by 15% 
-- ● Set an OUT parameter with a valid message

CREATE OR REPLACE PROCEDURE transfer_employee (
    p_emp_id      IN  NUMBER,
    p_new_dept_id IN  NUMBER,
    p_message     OUT VARCHAR2
) IS
    v_dept_exists NUMBER;
    v_hire_date   DATE;
    v_job_id      VARCHAR2(20);
    v_old_dept_id NUMBER;
BEGIN
    -- 1. Validate that the new department actually exists
    SELECT COUNT(*) INTO v_dept_exists 
    FROM departments 
    WHERE department_id = p_new_dept_id;

    IF v_dept_exists = 0 THEN
        p_message := 'Error: The specified new department does not exist.';
        RETURN; -- Exit the procedure early
    END IF;

    -- 2. Fetch employee details (This automatically checks if the employee exists)
    BEGIN
        SELECT hire_date, job_id, department_id
        INTO v_hire_date, v_job_id, v_old_dept_id
        FROM employees
        WHERE employee_id = p_emp_id;
    EXCEPTION
        WHEN NO_DATA_FOUND THEN
            p_message := 'Error: Employee ID ' || p_emp_id || ' does not exist.';
            RETURN; -- Exit the procedure early
    END;

    -- 3. Save current job details into JOB_HISTORY
    -- (Assuming hire_date is used as the start_date for their current role)
    INSERT INTO job_history (employee_id, start_date, end_date, job_id, department_id)
    VALUES (p_emp_id, v_hire_date, SYSDATE, v_job_id, v_old_dept_id);

    -- 4. Move employee to new department and increase their salary by 15%
    UPDATE employees
    SET department_id = p_new_dept_id,
        salary = salary * 1.15
    WHERE employee_id = p_emp_id;

    -- 5. Set the success message
    p_message := 'Success: Employee transferred successfully with a 15% salary increase.';

EXCEPTION
    WHEN OTHERS THEN
        -- Catch-all for any other unexpected database errors
        p_message := 'An unexpected system error occurred: ' || SQLERRM;
END;
/
