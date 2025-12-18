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
    avatarid int not null default 1 comment '头像编号，默认值为 1',
    balance decimal(10, 2) not null default 0.00 comment '账户余额'
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
    remaining_seats int not null comment '剩余座位 (实时更新)'
);

-- 订单表
create table if not exists order_info (
    order_id int primary key auto_increment comment '订单号',
    user_id int not null comment '用户ID',
    flight_id varchar(20) not null comment '航班号',
    passenger_name varchar(32) not null comment '乘客姓名',
    passenger_idcard varchar(18) not null comment '乘客身份证号',
    departure_city varchar(50) not null comment '出发城市',
    arrival_city varchar(50) not null comment '到达城市',
    departure_time datetime not null comment '起飞时间',
    arrival_time datetime not null comment '到达时间',
    price decimal(10, 2) not null comment '票价',
    order_time datetime not null default CURRENT_TIMESTAMP comment '下单时间',
    order_status varchar(10) not null default '已支付' comment '订单状态：已支付/已取消',
    constraint chk_order_status check (order_status in ('已支付', '已取消')),
    foreign key (user_id) references user_info(id) on delete cascade,
    foreign key (flight_id) references flight_info(flight_id) on delete cascade
) comment '订单信息表';

-- 收藏表
create table if not exists favorite_info (
    fav_id int primary key auto_increment comment '收藏 ID',
    user_id int not null comment '用户ID',
    flight_id varchar(20) not null comment '航班号',
    create_time datetime not null default CURRENT_TIMESTAMP comment '收藏时间',
    unique key uniq_user_flight (user_id, flight_id),
    foreign key (user_id) references user_info(id) on delete cascade,
    foreign key (flight_id) references flight_info(flight_id) on delete cascade
);

-- 测试数据
insert user_info (username, password, phone, email, realname, idcard) values ('test', '123456', '13800138000', 'test@email.com', '张三', '123456789012345678');
insert admin_info (username, password) values ('admin', '123456');

insert into flight_info values ('国航CA8394', '中国国航', '北京', '北京大兴机场', '2026-01-01 10:30:00', '广州', '广州白云机场', '2026-01-01 15:00:00', 455, 80, 33);
insert into flight_info values ('南航CZ3102', '中国南方航空', '广州', '广州白云机场', '2026-01-01 09:20:00', '北京', '北京大兴机场', '2026-01-01 12:50:00', 410, 78, 28);
insert into flight_info values ('深航ZH9404', '深圳航空', '深圳', '深圳宝安机场', '2026-01-01 13:10:00', '成都', '成都天府机场', '2026-01-01 16:30:00', 620, 90, 37);
insert into flight_info values ('国航CA1406', '中国国航', '成都', '成都双流机场', '2026-01-01 15:30:00', '北京', '北京首都机场', '2026-01-01 18:45:00', 615, 86, 22);
insert into flight_info values ('东航MU2307', '东方航空', '上海', '上海虹桥机场', '2026-01-01 17:15:00', '广州', '广州白云机场', '2026-01-01 19:30:00', 135, 79, 41);
insert into flight_info values ('深航ZH1610', '深圳航空', '北京', '北京大兴机场', '2026-01-01 19:00:00', '深圳', '深圳宝安机场', '2026-01-01 22:25:00', 625, 89, 15);
-- 新增26条（总计32条）
-- 2026年1月
insert into flight_info values ('海航HU7101', '海南航空', '北京', '北京首都机场', '2026-01-05 07:00:00', '海口', '海口美兰机场', '2026-01-05 11:20:00', 880, 95, 45);
insert into flight_info values ('厦航MF8302', '厦门航空', '厦门', '厦门高崎机场', '2026-01-08 08:15:00', '上海', '上海浦东机场', '2026-01-08 10:40:00', 350, 82, 31);
insert into flight_info values ('川航3U8888', '四川航空', '成都', '成都天府机场', '2026-01-10 11:40:00', '重庆', '重庆江北机场', '2026-01-10 13:10:00', 180, 75, 29);
insert into flight_info values ('吉祥HO1203', '吉祥航空', '上海', '上海浦东机场', '2026-01-12 14:20:00', '南京', '南京禄口机场', '2026-01-12 15:50:00', 150, 70, 38);
insert into flight_info values ('国航CA930', '中国国航', '北京', '北京大兴机场', '2026-01-15 16:30:00', '巴黎', '巴黎戴高乐机场', '2026-01-15 22:10:00', 5800, 280, 120); -- 国际航班（虚构）
insert into flight_info values ('南航CZ6108', '中国南方航空', '沈阳', '沈阳桃仙机场', '2026-01-18 18:00:00', '广州', '广州白云机场', '2026-01-18 21:30:00', 750, 88, 18);
insert into flight_info values ('东航MU5501', '东方航空', '西安', '西安咸阳机场', '2026-01-20 20:10:00', '上海', '上海虹桥机场', '2026-01-20 22:45:00', 420, 85, 27);
insert into flight_info values ('深航ZH8705', '深圳航空', '深圳', '深圳宝安机场', '2026-01-22 23:00:00', '武汉', '武汉天河机场', '2026-01-23 01:40:00', 390, 72, 12); -- 凌晨航班
insert into flight_info values ('海航HU7306', '海南航空', '杭州', '杭州萧山机场', '2026-01-25 05:30:00', '北京', '北京首都机场', '2026-01-25 08:00:00', 480, 86, 40); -- 早凌晨航班
insert into flight_info values ('厦航MF8501', '厦门航空', '福州', '福州长乐机场', '2026-01-28 10:00:00', '深圳', '深圳宝安机场', '2026-01-28 12:30:00', 520, 89, 35);
insert into flight_info values ('川航3U6709', '四川航空', '重庆', '重庆江北机场', '2026-01-30 13:45:00', '昆明', '昆明长水机场', '2026-01-30 16:15:00', 650, 92, 48);
insert into flight_info values ('吉祥HO1808', '吉祥航空', '南京', '南京禄口机场', '2026-01-31 17:20:00', '青岛', '青岛胶东机场', '2026-01-31 19:00:00', 320, 78, 24);
-- 2026年2月（共14条，覆盖全月）
insert into flight_info values ('国航CA1509', '中国国航', '北京', '北京首都机场', '2026-02-02 06:40:00', '上海', '上海浦东机场', '2026-02-02 09:10:00', 380, 90, 52);
insert into flight_info values ('南航CZ3405', '中国南方航空', '广州', '广州白云机场', '2026-02-05 09:30:00', '成都', '成都双流机场', '2026-02-05 12:20:00', 550, 87, 33);
insert into flight_info values ('东航MU2901', '东方航空', '上海', '上海虹桥机场', '2026-02-08 11:10:00', '西安', '西安咸阳机场', '2026-02-08 14:00:00', 460, 84, 26);
insert into flight_info values ('深航ZH9503', '深圳航空', '北京', '北京大兴机场', '2026-02-10 14:50:00', '长沙', '长沙黄花机场', '2026-02-10 17:20:00', 510, 81, 39);
insert into flight_info values ('海航HU7502', '海南航空', '三亚', '三亚凤凰机场', '2026-02-12 16:40:00', '北京', '北京首都机场', '2026-02-12 20:50:00', 950, 98, 49);
insert into flight_info values ('厦航MF8706', '厦门航空', '上海', '上海浦东机场', '2026-02-15 19:15:00', '厦门', '厦门高崎机场', '2026-02-15 21:40:00', 380, 83, 22);
insert into flight_info values ('川航3U8633', '四川航空', '拉萨', '拉萨贡嘎机场', '2026-02-18 07:30:00', '成都', '成都天府机场', '2026-02-18 10:10:00', 780, 85, 19);
insert into flight_info values ('吉祥HO1609', '吉祥航空', '广州', '广州白云机场', '2026-02-20 12:20:00', '杭州', '杭州萧山机场', '2026-02-20 14:30:00', 310, 76, 41);
insert into flight_info values ('国航CA4208', '中国国航', '成都', '成都双流机场', '2026-02-22 15:40:00', '深圳', '深圳宝安机场', '2026-02-22 18:10:00', 630, 88, 34);
insert into flight_info values ('南航CZ8888', '中国南方航空', '重庆', '重庆江北机场', '2026-02-24 18:30:00', '广州', '广州白云机场', '2026-02-24 21:00:00', 480, 91, 28);
insert into flight_info values ('东航MU6102', '东方航空', '武汉', '武汉天河机场', '2026-02-25 21:00:00', '北京', '北京大兴机场', '2026-02-25 23:30:00', 520, 79, 17);
insert into flight_info values ('深航ZH8801', '深圳航空', '青岛', '青岛胶东机场', '2026-02-26 00:10:00', '上海', '上海浦东机场', '2026-02-26 02:40:00', 450, 73, 9); -- 凌晨航班
insert into flight_info values ('海航HU7809', '海南航空', '大连', '大连周水子机场', '2026-02-27 08:20:00', '海口', '海口美兰机场', '2026-02-27 12:50:00', 820, 94, 51);
insert into flight_info values ('厦航MF8901', '厦门航空', '北京', '北京首都机场', '2026-02-28 13:10:00', '厦门', '厦门高崎机场', '2026-02-28 16:00:00', 680, 89, 42);

-- 改签测试 (满足出发城市和到达城市相同)
insert into flight_info values ('东航MU5101', '东方航空', '北京', '北京首都机场', '2026-01-01 08:00:00', '上海', '上海浦东机场', '2026-01-01 10:15:00', 135, 85, 45);
insert into flight_info values ('东航MU5103', '东方航空', '北京', '北京大兴机场', '2026-01-01 11:30:00', '上海', '上海虹桥机场', '2026-01-01 13:45:00', 158, 88, 60);

-- 账户初始余额：测试用户 65 元
update user_info set balance = 65 where username = 'test';

-- 测试订单：东航MU5101，北京 -> 上海，乘客张三，票价 135，订单时间 2025-12-15 17:30
insert into order_info (user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time)
values (1, '东航MU5101', '张三', '123456789012345678', '北京', '上海', '2026-01-01 08:00:00', '2026-01-01 10:15:00', 135, '2025-12-15 17:30:00');

