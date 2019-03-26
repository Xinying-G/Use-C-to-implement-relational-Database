create index ON s(t_id);
create index ON s(u_id);	
create index ON r(s_id);
create index ON u(u_5);	
analyze r;
analyze u;
analyze s;	

explain
select t.*
from t
join s using (t_id)
join r using (s_id)
join u using (u_id)
where u_5 = 0;

drop index s_t_id_idx;
drop index r_s_id_idx;
drop index u_u_5_idx;
drop index s_u_id_idx;