--1  Write a PL/SQL procedure named LONGEST_SERVING_EMPLOYEE that takes a
-- REGION_NAME as input and identifies the employee who has been working for the longest time within
-- that region.

CREATE OR REPLACE PROCEDURE LONGEST_SERVING_EMPLOYEE (REGION_NAME IN VARCHAR2) IS
V_FULLNAME VARCHAR2(100);
V_JOB_TITLE VARCHAR2(50);
V_HIRE_DATE DATE;
V_COUNTRY_NAME VARCHAR2(50);
V_DEPARTMENT_NAME VARCHAR2(50);
V_CITY_NAME VARCHAR2(50);
BEGIN
    SELECT E.FIRST_NAME || ' ' || E.LAST_NAME,J.JOB_TITLE,E.HIRE_DATE,C.COUNTRY_NAME,D.DEPARTMENT_NAME,L.CITY
    INTO V_FULLNAME, V_JOB_TITLE, V_HIRE_DATE, V_COUNTRY_NAME, V_DEPARTMENT_NAME, V_CITY_NAME
    FROM EMPLOYEES E
    JOIN JOBS J ON E.JOB_ID=J.JOB_ID
    JOIN DEPARTMENTS D ON E.DEPARTMENT_ID=D.DEPARTMENT_ID   
    JOIN LOCATIONS L ON D.LOCATION_ID=L.LOCATION_ID
    JOIN COUNTRIES C ON L.COUNTRY_ID=C.COUNTRY_ID
    JOIN REGIONS R ON C.REGION_ID=R.REGION_ID
    WHERE R.REGION_NAME=REGION_NAME
    ORDER BY E.HIRE_DATE ASC
    FETCH FIRST 1 ROWS ONLY;
    DBMS_OUTPUT.PUT_LINE('Longest Serving Employee in ' || REGION_NAME || ': ' || V_FULLNAME);
    DBMS_OUTPUT.PUT_LINE('Job Title: ' || V_JOB_TITLE);
    DBMS_OUTPUT.PUT_LINE('Hire Date: ' || TO_CHAR(V_HIRE_DATE, 'DD-MON-YYYY'));
    DBMS_OUTPUT.PUT_LINE('Country: ' || V_COUNTRY_NAME);
    DBMS_OUTPUT.PUT_LINE('Department: ' || V_DEPARTMENT_NAME);
    DBMS_OUTPUT.PUT_LINE('City: ' || V_CITY_NAME);
    EXCEPTION
    WHEN NO_DATA_FOUND THEN
        DBMS_OUTPUT.PUT_LINE('No employees found in region: ' || REGION_NAME);
    WHEN TOO_MANY_ROWS THEN
        DBMS_OUTPUT.PUT_LINE('Multiple employees share the same earliest hire date in region: ' || REGION_NAME);
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Unexpected error');

END;
/

BEGIN
    LONGEST_SERVING_EMPLOYEE('Americas');
END;



--2

CREATE OR REPLACE PROCEDURE RANK_JOBS IS
RANK_NO NUMBER;
BEGIN 
    FOR REC IN(
    SELECT 
    RANK() OVER(
       ORDER BY COUNT(E.EMPLOYEE_ID) DESC,
       AVG(E.SALARY) DESC
    ) AS RANK_NO,
    J.JOB_TITLE,
    COUNT(E.EMPLOYEE_ID) AS EMP_COUNT,
    AVG(E.SALARY) AS AVG_SAL,
    MAX(E.SALARY) AS MAX_SAL,
    MIN(E.SALARY) AS MIN_SAL
    FROM EMPLOYEES E 
    JOIN JOBS J ON E.JOB_ID=J.JOB_ID
    GROUP BY J.JOB_ID,J.JOB_TITLE
    )
    LOOP 
        DBMS_OUTPUT.PUT_LINE(
            'Rank: ' || REC.RANK_NO ||
            ' | Job: ' || REC.JOB_TITLE ||
            ' | Employees: ' || REC.EMP_COUNT ||
            ' | Avg Salary: ' || REC.AVG_SAL ||
            ' | Max Salary: ' || REC.MAX_SAL ||
            ' | Min Salary: ' || REC.MIN_SAL
        );
    END LOOP;
END;
/

BEGIN 
    RANK_JOBS;
END;
/


--3

CREATE TABLE LEAVES(
    EMPLOYEE_ID NUMBER,
    EMPLOYEE_WORKING_INSTEAD_OF NUMBER,
    CUR_DATE DATE
);
CREATE OR REPLACE TRIGGER EMPLOYEE_LEAVE
BEFORE DELETE ON EMPLOYEES
FOR EACH ROW
DECLARE
    v_replacement_emp EMPLOYEES.EMPLOYEE_ID%TYPE;
    v_old_manager     EMPLOYEES.MANAGER_ID%TYPE;
    v_sub_count       NUMBER;
BEGIN
    -- Initialize
    v_replacement_emp := NULL;
    v_old_manager := :OLD.MANAGER_ID;

    -- Count subordinates to see if the employee is a manager
    SELECT COUNT(*) INTO v_sub_count
    FROM EMPLOYEES
    WHERE MANAGER_ID = :OLD.EMPLOYEE_ID;

    ------------------------------------------------
    -- CONDITION 2: If employee is a manager
    -- Find another manager with closest subordinate count
    ------------------------------------------------
    IF v_sub_count > 0 THEN
        BEGIN
            SELECT EMPLOYEE_ID
            INTO v_replacement_emp
            FROM (
                SELECT E.EMPLOYEE_ID,
                       ABS(
                           (SELECT COUNT(*) 
                            FROM EMPLOYEES 
                            WHERE MANAGER_ID = E.EMPLOYEE_ID) - v_sub_count
                       ) AS diff
                FROM EMPLOYEES E
                WHERE E.EMPLOYEE_ID != :OLD.EMPLOYEE_ID
                  AND EXISTS (SELECT 1 FROM EMPLOYEES S WHERE S.MANAGER_ID = E.EMPLOYEE_ID)
                ORDER BY diff
            )
            WHERE ROWNUM = 1;
        EXCEPTION
            WHEN NO_DATA_FOUND THEN
                v_replacement_emp := NULL;
        END;

    ELSE
        ------------------------------------------------
        -- CONDITION 1: Regular employee with a manager
        -- Find employee under same manager with closest salary
        ------------------------------------------------
        IF v_old_manager IS NOT NULL THEN
            BEGIN
                SELECT EMPLOYEE_ID
                INTO v_replacement_emp
                FROM (
                    SELECT EMPLOYEE_ID
                    FROM EMPLOYEES
                    WHERE MANAGER_ID = v_old_manager
                      AND EMPLOYEE_ID != :OLD.EMPLOYEE_ID
                    ORDER BY ABS(SALARY - :OLD.SALARY)
                )
                WHERE ROWNUM = 1;
            EXCEPTION
                WHEN NO_DATA_FOUND THEN
                    v_replacement_emp := NULL;
            END;
        END IF;
    END IF;

    ------------------------------------------------
    -- Insert record into LEAVES table
    ------------------------------------------------
    INSERT INTO LEAVES (
        EMPLOYEE_ID, 
        EMPLOYEE_WORKING_INSTEAD_OF, 
        CUR_DATE
    )
    VALUES (
        :OLD.EMPLOYEE_ID, 
        v_replacement_emp, 
        SYSDATE
    );

EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Error in EMPLOYEE_LEAVE trigger: ' || SQLERRM);

END;
/