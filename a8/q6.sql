create index ON r(r_1p);
create index ON r(r_10);
-- create index ON r(r_1p,r_10);
analyze r;

-- explain
-- select * 
-- from r
-- where r_1p = 0
-- union
-- select *
-- from r
-- where r_10 = 0;

explain
select * 
from r
where r_1p = 0 OR r_10 = 0;
-- union
-- select * 
-- from r
-- where r_1p = 0 AND r_10 = 0;


drop index r_r_10_idx;
drop index r_r_1p_idx;
-- drop index r_r_1p_r_10_idx;	