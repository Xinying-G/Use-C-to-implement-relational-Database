-- Store in the routing table the rows (from_member_id, to_member_id, x), 
-- for each message id x in message_ids.
-- The return value should be the number of rows inserted.

drop function if exists store_routing();
create or replace function store_routing(
    from_member_id int, 
    to_member_id int, 
    message_ids int[]) returns int as $$
declare
    mid int := array_length(message_ids,1);
    insert_count int = 1;
begin 
    -- while insert_count <= mid LOOP
    -- RAISE NOTICE 'from member id %', from_member_id;
    -- RAISE NOTICE 'to member id %', to_member_id;
    -- RAISE NOTICE 'count %', mid;
        
    insert into routing(from_member_id, to_member_id, message_id) 
    		select from_member_id, to_member_id,unnest(message_ids);
    return mid;
end
$$ language plpgsql;
