create index ON s(t_id);
create index ON r(s_id);
create index ON s(s_20);
create index ON t(t_5);
analyze r;
analyze s;
analyze t;

explain
select *
from r 
join s using (s_id)
join t using (t_id)
where s_20 = 0 and t_5 = 0;

drop index s_s_20_idx;
drop index s_t_id_idx;
drop index r_s_id_idx;
drop index t_t_5_idx;