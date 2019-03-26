-- select *
-- from r
-- where r_10p between 1 and 2;

-- select 'ORIGINAL -- NO INDEXES';

-- explain
-- select *
-- from r
-- where r_10p between 1 and 2;

-- select 'ADD INDEX ON r_10P';

-- create index on r(r_10p);
-- analyze r;

-- explain
-- select *
-- from r
-- where r_10p between 1 and 2;

-- drop index r_r_10p_idx;

create index ON r(r_2);
create index ON r(r_1p);
create index ON r(r_50p);
analyze r;	

explain
select * from r where r_2 = 0;
explain
select * from r where r_1p = 0;
explain
select * from r where r_50p = 0;

drop index r_r_2_idx;
drop index r_r_1p_idx;
drop index r_r_50p_idx;