create index ON s(s_4);
create index ON r(s_id,r_50p);
analyze r;
analyze s;	

explain
select r_10, s_20
from r
join s using (s_id)
where r_50p = 0
and s_4 = 0;

drop index r_s_id_r_50p_idx;
drop index s_s_4_idx;