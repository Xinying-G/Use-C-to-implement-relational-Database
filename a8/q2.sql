create index ON r(r_1p,r_10p,r_50p);
analyze r;	

explain
select * from r 
where r_1p = 0
and r_10p = 0
and r_50p = 0;

drop index r_r_1p_r_10p_r_50p_idx;