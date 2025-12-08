-- 创建数据库
create database if not exists flient_management;

use flient_management;

-- 用户表
create table if not exists user_info (
	id int primary key auto_increment comment 'id',
    username varchar(32) not null unique comment '用户名',
    password varchar(32) not null comment '密码',
    phone varchar(11) not null unique comment '手机号',
    email varchar(64) not null unique comment '邮箱',
    realname varchar(32) not null comment '真实姓名',
    idcard varchar(18) not null unique comment '身份证号',
    avatarid int not null default 1 comment '头像编号，默认值为 1'
);

-- 管理员表
create table if not exists admin_info (
    id int primary key auto_increment comment 'id',
    username varchar(32) not null unique comment '用户名',
    password varchar(32) not null comment '密码'
);

-- 航班信息表
create table if not exists flight_info (
    flight_id varchar(20) primary key not null comment '航班号',
    airline_company varchar(50) not null comment '航空公司',
    departure_city varchar(50) not null comment '出发城市',
    departure_airport varchar(50) not null comment '出发机场',
    departure_time datetime not null comment '出发时间',
    arrival_city varchar(50) not null comment '到达城市',
    arrival_airport varchar(50) not null comment '到达机场',
    arrival_time datetime not null comment '到达时间',
    price decimal(10, 2) not null comment '价格',
    total_seats int not null comment '总座位数',
    remaining_seats int not null comment '剩余座位（实时更新)'
);

-- 测试数据
insert user_info (username, password, phone, email, realname, idcard) values ('test', '123456', '13800138000', 'test@email.com', '张三', '123456789012345678');
insert admin_info (username, password) values ('admin', '123456');

insert into flight_info values ('国航CA8394', '中国国航', '北京', '北京大兴机场', '2026-01-01 10:30:00', '广州', '广州白云机场', '2026-01-01 15:00:00', 455, 80, 33);
insert into flight_info values ('东航MU5101', '东方航空', '北京', '北京首都机场', '2026-01-01 08:00:00', '上海', '上海浦东机场', '2026-01-01 10:15:00', 135, 85, 45);
insert into flight_info values ('南航CZ3102', '中国南方航空', '广州', '广州白云机场', '2026-01-01 09:20:00', '北京', '北京大兴机场', '2026-01-01 12:50:00', 410, 78, 28);
insert into flight_info values ('深航ZH9404', '深圳航空', '深圳', '深圳宝安机场', '2026-01-01 13:10:00', '成都', '成都天府机场', '2026-01-01 16:30:00', 620, 90, 37);
insert into flight_info values ('国航CA1406', '中国国航', '成都', '成都双流机场', '2026-01-01 15:30:00', '北京', '北京首都机场', '2026-01-01 18:45:00', 615, 86, 22);
insert into flight_info values ('东航MU2307', '东方航空', '上海', '上海虹桥机场', '2026-01-01 17:15:00', '广州', '广州白云机场', '2026-01-01 19:30:00', 135, 79, 41);
insert into flight_info values ('深航ZH1610', '深圳航空', '北京', '北京大兴机场', '2026-01-01 19:00:00', '深圳', '深圳宝安机场', '2026-01-01 22:25:00', 625, 89, 15);
