--1

-- CREATE TABLE EMPLOYEES_COPY AS
-- SELECT * FROM EMPLOYEES;
-- COMMIT;

create or replace procedure show_employee_info (
   eid in varchar2
) is
   ename employees.first_name%type;
   ejob  employees.job_id%type;
   esal  employees.salary%type;
   emid  employees.manager_id%type;
   edept employees.department_id%type;
begin
   select first_name,
          job_id,
          salary,
          manager_id,
          department_id
     into
      ename,
      ejob,
      esal,
      emid,
      edept
     from employees_copy
    where employee_id = eid;

   dbms_output.put_line('EMPLOYYE INFO:');
   dbms_output.put_line('ID: ' || eid);
   dbms_output.put_line('NAME: ' || ename);
   dbms_output.put_line('JOB_ID: ' || ejob);
   dbms_output.put_line('MANAGER_ID: ' || emid);
   dbms_output.put_line('DEPT_ID: ' || edept);
end;
/

create or replace function exchange_employees (
   mid1 in number,
   mid2 in number
) return varchar2 is
   emp1_id     employees.employee_id%type;
   emp2_id     employees.employee_id%type;
   emp1_salary employees.salary%type;
   emp2_salary employees.salary%type;
   emp1_dept   employees.department_id%type;
   emp2_dept   employees.department_id%type;
begin
   select employee_id,
          salary,
          department_id
     into
      emp1_id,
      emp1_salary,
      emp1_dept
     from employees
    where manager_id = mid1
    order by salary asc
    fetch first 1 rows only;

   select employee_id,
          salary,
          department_id
     into
      emp2_id,
      emp2_salary,
      emp2_dept
     from employees
    where manager_id = mid2
    order by salary asc
    fetch first 1 rows only;

   show_employee_info(emp1_id);
   show_employee_info(emp2_id);
   update employees_copy
      set department_id = emp2_dept,
          manager_id = mid2,
          salary = salary + abs(emp1_salary - emp2_salary) * 0.5
    where employee_id = emp1_id;

   update employees_copy
      set department_id = emp1_dept,
          manager_id = mid1,
          salary = salary + abs(emp1_salary - emp2_salary) * 0.5
    where employee_id = emp2_id;

   show_employee_info(emp1_id);
   show_employee_info(emp2_id);
   return 'Exchange Completed Successfully.';
end;
/

declare
   v_output varchar2(4000);
begin
   v_output := exchange_employees(
      100,
      145
   );
   dbms_output.put_line(v_output);
end;
/

--2 SELECT RANK,EMPLOYEE COUNT ,AVG SALARY OF THEM AND HIGHEST PAID JOB TITEL FOR EACH LOCATION

create or replace procedure location_salary_report is
   city_rank number;
begin
   for rec in (
      select rank()
             over(
          order by count(e.employee_id) asc,
                   round(
                          avg(e.salary),
                          2
                       ) desc
             ) as city_rank,
             l.city,
             count(e.employee_id) as emp_count,
             round(
                avg(e.salary),
                2
             ) as avg_sal,
             (
                select j.job_title
                  from employees e2
                  join jobs j
                on e2.job_id = j.job_id
                  join departments d2
                on d2.department_id = e2.department_id
                 where d2.location_id = l.location_id
                 order by e2.salary desc
                 fetch first 1 row only
             ) as hpj
        from employees e
        join departments d
      on e.department_id = d.department_id
        join locations l
      on d.location_id = l.location_id
       group by l.city,
                l.location_id
   ) loop
      dbms_output.put_line('Rank: ' || rec.city_rank);
      dbms_output.put_line('City: ' || rec.city);
      dbms_output.put_line('Number of Employees: ' || rec.emp_count);
      dbms_output.put_line('Average Salary: ' || rec.avg_sal);
      dbms_output.put_line('Highest Paying Job Title: ' || rec.hpj);
      dbms_output.put_line('----------------------------------------');
   end loop;
end;
/

begin
   location_salary_report;
end;
/

--3
create table transfers (
   employee_id                     number,
   employee_working_instead_of_him number,
   new_department                  number,
   cur_date                        date
);

create or replace trigger new_transfer after
   update of department_id on employees
   for each row
   when ( old.department_id is not null )
declare
   v_replacement_emp employees.employee_id%type;
   v_old_manager     employees.manager_id%type;
   v_new_manager     employees.manager_id%type;
   v_old_salary      employees.salary%type;
begin
   if :old.department_id != :new.department_id then
      v_old_manager := :old.manager_id;
      v_old_salary := :old.salary;

        ------------------------------------------------
        -- CONDITION 1: IF EMPLOYEE HAD A MANAGER
        ------------------------------------------------
      if v_old_manager is not null then
         ------------------------------------------------
            -- (a) Find employee with closest salary
            ------------------------------------------------
         select employee_id
           into v_replacement_emp
           from (
            select employee_id
              from employees
             where manager_id = v_old_manager
               and employee_id != :old.employee_id
             order by abs(salary - v_old_salary)
         )
          where rownum = 1;
            ------------------------------------------------
            -- Increase replacement salary
            ------------------------------------------------
         update employees
            set
            salary = salary + ( 0.5 * v_old_salary )
          where employee_id = v_replacement_emp;

            ------------------------------------------------
            -- (b) Assign new manager in new department
            -- Find manager with closest number of subordinates
            ------------------------------------------------
         select manager_id
           into v_new_manager
           from (
            select manager_id,
                   abs((
                      select count(*)
                        from employees
                       where manager_id = e.manager_id
                   ) -(
                      select count(*)
                        from employees
                       where manager_id = v_old_manager
                   )) diff
              from employees e
             where department_id = :new.department_id
               and manager_id is not null
             group by manager_id
             order by diff
         )
          where rownum = 1;
            -- Update transferred employee’s manager
         update employees
            set
            manager_id = v_new_manager
          where employee_id = :new.employee_id;

      end if;
        ------------------------------------------------
        -- Insert into TRANSFERS table
        ------------------------------------------------
      insert into transfers values ( :old.employee_id,
                                     v_replacement_emp,
                                     :new.department_id,
                                     sysdate );

   end if;
end;
/

