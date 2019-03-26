-- Q0:
select name from member
except
select name
from member, message ,routing
where routing.to_member_id = routing.from_member_id
and routing.from_member_id = member.member_id
order by name;

-- Q1:
select birth_date 
from member
where name = 'Tweetii';

-- Q2
select distinct message_date
from routing,message,member
where member.name = 'Zyrianyhippy'
and member.member_id = routing.from_member_id
and routing.message_id = message.message_id
order by message_date desc;

--Q3:
-- select distinct name
-- from member, routing, message
-- where routing.to_member_id = member.member_id
-- and routing.message_id = message.message_id
-- and member.birth_date = message.message_date
-- order by name;


select name
from member, routing, message
where routing.to_member_id = member.member_id
and routing.message_id = message.message_id
and date_part('month',member.birth_date) = date_part('month',message.message_date)
and date_part('day',member.birth_date) = date_part('day',message.message_date)
order by name;

--Q4:
select distinct message_date 
from message, routing, member
where member.name = 'Unguiferous'
and member.member_id = routing.from_member_id
and message.message_id = routing.message_id
and routing.message_id in(
select routing.message_id
from routing, member
where member.name = 'Froglet'
and member.member_id = routing.to_member_id)
order by message_date;

--Q5:
select distinct member.name
from message, routing, member
where message.message_date = '2016/05/17'
and message.message_id = routing.message_id
and routing.from_member_id = member.member_id
order by member.name;

--Q6:
select distinct member.name
from routing, member
where routing.to_member_id = member.member_id
and routing.from_member_id in(
select routing.from_member_id
from member, routing
where member.name = 'Cephalophore'
and member.member_id = routing.from_member_id
)
order by member.name;

--Q7:
select 
  m1.name as sender_name,
  m2.name as reciever_name
from routing ro, message m, member m1, member m2
where m.message_date = '2016/05/17'
and ro.message_id = m.message_id
and m1.member_id = ro.from_member_id
and m2.member_id = ro.to_member_id
order by sender_name,reciever_name;

--Q8:
-- select to_member_id, count(*) 
-- from routing ro, member m
-- where ro.from_member_id = m.member_id
-- and m.name = 'Lucarne'
-- group by ro.to_member_id;

-- select m2.member_id, count(*)
select m2.name, count(q.name) 
from 
(select *
from routing ro join member m1 on ro.from_member_id = m1.member_id
where ro.from_member_id = m1.member_id 
and m1.name = 'Lucarne') as q
right join member m2 on m2.member_id = to_member_id
where m2.name != 'Lucarne'
group by m2.name
order by count desc;

--Q9:

-- select ro.from_member_id, ro.message_id, m1.name, count(*)
-- from routing ro join member m1 on ro.from_member_id = m1.member_id
-- group by ro.from_member_id, ro.message_id, m1.name;


select q.name,count(q.message_id)
from (
select ro.from_member_id, ro.message_id, m1.name
from routing ro join member m1 on ro.from_member_id = m1.member_id
group by ro.from_member_id, ro.message_id, m1.name
) as q
group by q.from_member_id, q.name
having count(q.message_id) >= 20
order by count(*) desc;

--Q10: 
select min(birth_date), max(birth_date)
from member;

--Q11:
select count(*)
from routing ro, member m 
where ro.to_member_id = m.member_id
and m.name = 'Abderian';

--Q12:
  select name,
    min(message_date) as oldest_message,
    max(message_date) as newest_message
  from member mem left join routing ro on ro.from_member_id = mem.member_id
  left join message m on m.message_id = ro.message_id 
  group by member_id,name
  order by name;

-- select max(message.message_date), min(message.message_date) 
-- from(
--   select member_id
--   from routing ro, member m
--   where ro.from_member_id = m.member_id
--   group by member_id
-- )

--Q13:
select distinct p.message_date 
from (select *
from routing, member, message 
where routing.message_id = message.message_id
and routing.from_member_id = member.member_id
and member.name = 'Abderian') as q join
(select *
from routing, member, message 
where routing.message_id = message.message_id
and routing.from_member_id = member.member_id
and member.name = 'Unguiferous') as p on q.message_date = p.message_date
order by p.message_date;

--Q14:
select name
from member 
where birth_date is null
order by name;

--Q15:
select name
from member m left join routing on m.member_id = routing.from_member_id
where routing.from_member_id is null
order by name;
















