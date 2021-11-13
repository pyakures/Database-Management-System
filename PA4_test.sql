-- CS457 PA4

-- This script includes the commands to be executed by two processes, P1 and P2

-- On P1:
CREATE DATABASE CS457_PA4;
USE CS457_PA4;
create table Flights(seat int, status int);
insert into Flights values(22,0); -- seat 22 is available
insert into Flights values(23,1); -- seat 23 is occupied
begin transaction;
update flights set status = 1 where seat = 22;

-- On P2:
USE CS457_PA4;
select * from Flights;
begin transaction;
update flights set status = 1 where seat = 22;
commit; --there should be nothing to commit; it's an "abort"
select * from Flights;

-- On P1:
commit; --persist the change to disk
select * from Flights;

-- On P2:
select * from Flights;

---------------------
-- Expected output --
---------------------

-- On P1:
-- Database CS457_PA4 created.
-- Using database CS457_PA4.
-- Table Flights created.
-- 1 new record inserted.
-- 1 new record inserted.
-- Transaction starts.
-- 1 record modified.
-- Transaction committed.
-- seat int|status int
-- 22|1
-- 23|1


-- On P2:
-- Using database CS457_PA4.
-- seat int|status int
-- 22|0
-- 23|1
-- Transaction starts.
-- Error: Table Flights is locked!
-- Transaction abort.
-- seat int|status int
-- 22|0
-- 23|1
-- seat int|status int
-- 22|1
-- 23|1