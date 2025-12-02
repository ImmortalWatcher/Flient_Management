-- 创建数据库
create database if not exists flient_management;

use flient_management;

-- 用户表
create table if not exists user_info (
	id int primary key auto_increment comment 'id',
    username varchar(32) not null unique comment '用户名',
    password varchar(32) not null comment '密码'
    --新增
    real_name varchar(50) not null comment '真实姓名（购票需实名）',
    id_card varchar(18) not null unique comment '身份证号码（唯一，实名验证）',
    phone varchar(11) not null comment '电话号码',
    email varchar(100) not null comment '邮箱'
);

-- 测试数据 (用户)
insert user_info (username, password) values ('test', '123456');

-- 管理员表
create table if not exists admin_info(
    id int primary key auto_increment comment '管理员id',
    adminname varchar(50) not null comment '用户名（账号）',
    password varchar(100) not null comment '密码',
    real_name varchar(50) not null comment '真实姓名',
    id_card varchar(18) not null unique comment '身份证号码',
    phone varchar(11) not null comment '电话号码',
    email varchar(100) not null comment '邮箱'
);

--航班信息表
create table if not exists flight(
    flight_id varchar(20) primary key not null comment '航班号',
    airline_company varchar(50) not null comment '航空公司',
    departure_city varchar(50) not null comment '出发城市',
    departure_airport varchar(50) not null comment '出发机场',
    departure_time datetime not null comment '出发时间',
    arrival_city varchar(50) not null comment '到达城市',
    arrival_airport varchar(50) not null comment '到达机场',
    arrival_time datetime not null comment '到达时间',
    price decimal(10,2) not null comment '价格',
    total_seats int not null comment '总座位数',
    remaining_seats int not null comment '剩余座位（实时更新）',
    flight_status tinyint default 1 comment '航班状态:1-正常，2-延误，3-取消，4-已起飞，5-已降落',
    create_time datetime default current_timestamp comment '添加时间',
    update_time datetime default current_timestamp on update current_timestamp comment '更新时间',
    index idx_departure(departure_city,departure_time) comment'按出发城市+时间查询索引',
    index idx_arrival(arrival_city,arrival_time) comment '按到达城市+时间查询索引'
);

-- 个人订单表（order）
create table personal_order (
  order_id varchar(32) primary key comment '订单号（主键，如：OCD20240520123456）',
  user_id int not null comment '关联用户id（外键）',
  flight_id varchar(20) not null comment '关联航班号（外键）',
  total_price decimal(10,2) not null comment '订单总金额',
  order_status tinyint not null comment '订单状态：1-待支付，2-已支付,3-已取消',
  payment_time datetime comment '支付时间',
  create_time datetime default current_timestamp comment '下单(预订)时间',
  foreign key (user_id) references user_info(id) on delete cascade comment '用户删除时，订单同步删除',
  foreign key (flight_id) references flight(flight_id) on delete restrict comment '航班存在时，才能创建订单',
  index idx_user (user_id) comment '按用户id查询订单索引',
  index idx_flight (flight_id) comment '按航班号查询订单索引',
  index idx_status (order_status) comment '按订单状态查询索引'
);
