-- 创建数据库
create database if not exists flient_management;

use flient_management;

-- 用户表
create table if not exists user_info (
    id int primary key auto_increment comment '用户 ID',
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
    id int primary key auto_increment comment '管理员 ID',
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
    order_id int primary key auto_increment comment '订单序号',
    order_no varchar(17) unique comment '订单号',
    user_id int not null comment '用户ID',
    flight_id varchar(20) not null comment '航班号',
    passenger_name varchar(32) not null comment '乘客姓名',
    passenger_idcard varchar(18) not null comment '乘客身份证号',
    departure_city varchar(50) not null comment '出发城市',
    arrival_city varchar(50) not null comment '到达城市',
    departure_time datetime not null comment '起飞时间',
    arrival_time datetime not null comment '到达时间',
    price decimal(10, 2) not null comment '票价',
    order_time datetime not null default current_timestamp comment '下单时间',
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
    create_time datetime not null default current_timestamp comment '收藏时间',
    unique key uniq_user_flight (user_id, flight_id),
    foreign key (user_id) references user_info(id) on delete cascade,
    foreign key (flight_id) references flight_info(flight_id) on delete cascade
);

-- ========== 测试数据 ==========

-- 用户数据
insert into user_info (username, password, phone, email, realname, idcard, avatarid, balance) values ('test', '123456', '13800138000', 'test@email.com', '张三', '123456789012345678', 3, 200);
insert into user_info (username, password, phone, email, realname, idcard, avatarid, balance) values ('alice', '123456', '13912345678', 'alice.wang@qq.com', '王爱丽', '110101199001011234', 12, 1580);
insert into user_info (username, password, phone, email, realname, idcard, avatarid, balance) values ('bob', '123456', '15098765432', 'bob.li@163.com', '李波', '320102199203154567', 7, 45);
insert into user_info (username, password, phone, email, realname, idcard, avatarid, balance) values ('charlie', '123456', '18612345678', 'charlie.zhang@gmail.com', '张查理', '440103199505207890', 15, 890);
insert into user_info (username, password, phone, email, realname, idcard, avatarid, balance) values ('david', '123456', '13765432109', 'david.zhao@sina.com', '赵大卫', '510104199208081234', 4, 1230);
insert into user_info (username, password, phone, email, realname, idcard, avatarid, balance) values ('emily', '123456', '15987654321', 'emily.chen@outlook.com', '陈艾米', '330106199401121567', 9, 320);
insert into user_info (username, password, phone, email, realname, idcard, avatarid, balance) values ('frank', '123456', '18812345678', 'frank.liu@hotmail.com', '刘弗兰克', '420102199612253456', 2, 1680);
insert into user_info (username, password, phone, email, realname, idcard, avatarid, balance) values ('grace', '123456', '13698765432', 'grace.wu@foxmail.com', '吴格蕾丝', '500103199709088901', 11, 75);

-- 管理员数据
insert into admin_info (username, password) values ('admin', '123456');

-- 航班数据
insert into flight_info values ('国航CA8394', '中国国航', '北京', '北京大兴机场', '2026-01-01 10:30:00', '广州', '广州白云机场', '2026-01-01 15:00:00', 455, 80, 33);
insert into flight_info values ('南航CZ3102', '中国南方航空', '广州', '广州白云机场', '2026-01-01 09:20:00', '北京', '北京大兴机场', '2026-01-01 12:50:00', 410, 78, 28);
insert into flight_info values ('深航ZH9404', '深圳航空', '深圳', '深圳宝安机场', '2026-01-01 13:10:00', '成都', '成都天府机场', '2026-01-01 16:30:00', 620, 90, 37);
insert into flight_info values ('国航CA1406', '中国国航', '成都', '成都双流机场', '2026-01-01 15:30:00', '北京', '北京首都机场', '2026-01-01 18:45:00', 615, 86, 22);
insert into flight_info values ('东航MU2307', '东方航空', '上海', '上海虹桥机场', '2026-01-01 17:15:00', '广州', '广州白云机场', '2026-01-01 19:30:00', 135, 79, 41);
insert into flight_info values ('深航ZH1610', '深圳航空', '北京', '北京大兴机场', '2026-01-01 19:00:00', '深圳', '深圳宝安机场', '2026-01-01 22:25:00', 625, 89, 15);
insert into flight_info values ('海航HU7101', '海南航空', '北京', '北京首都机场', '2026-01-05 07:00:00', '海口', '海口美兰机场', '2026-01-05 11:20:00', 880, 95, 45);
insert into flight_info values ('厦航MF8302', '厦门航空', '厦门', '厦门高崎机场', '2026-01-08 08:15:00', '上海', '上海浦东机场', '2026-01-08 10:40:00', 350, 82, 31);
insert into flight_info values ('川航3U8888', '四川航空', '成都', '成都天府机场', '2026-01-10 11:40:00', '重庆', '重庆江北机场', '2026-01-10 13:10:00', 180, 75, 29);
insert into flight_info values ('吉祥HO1203', '吉祥航空', '上海', '上海浦东机场', '2026-01-12 14:20:00', '南京', '南京禄口机场', '2026-01-12 15:50:00', 150, 70, 38);
insert into flight_info values ('国航CA930', '中国国航', '北京', '北京大兴机场', '2026-01-15 16:30:00', '巴黎', '巴黎戴高乐机场', '2026-01-15 22:10:00', 5800, 280, 120);
insert into flight_info values ('南航CZ6108', '中国南方航空', '沈阳', '沈阳桃仙机场', '2026-01-18 18:00:00', '广州', '广州白云机场', '2026-01-18 21:30:00', 750, 88, 18);
insert into flight_info values ('东航MU5501', '东方航空', '西安', '西安咸阳机场', '2026-01-20 20:10:00', '上海', '上海虹桥机场', '2026-01-20 22:45:00', 420, 85, 27);
insert into flight_info values ('深航ZH8705', '深圳航空', '深圳', '深圳宝安机场', '2026-01-22 23:00:00', '武汉', '武汉天河机场', '2026-01-23 01:40:00', 390, 72, 12);
insert into flight_info values ('海航HU7306', '海南航空', '杭州', '杭州萧山机场', '2026-01-25 05:30:00', '北京', '北京首都机场', '2026-01-25 08:00:00', 480, 86, 40);
insert into flight_info values ('厦航MF8501', '厦门航空', '福州', '福州长乐机场', '2026-01-28 10:00:00', '深圳', '深圳宝安机场', '2026-01-28 12:30:00', 520, 89, 35);
insert into flight_info values ('川航3U6709', '四川航空', '重庆', '重庆江北机场', '2026-01-30 13:45:00', '昆明', '昆明长水机场', '2026-01-30 16:15:00', 650, 92, 48);
insert into flight_info values ('吉祥HO1808', '吉祥航空', '南京', '南京禄口机场', '2026-01-31 17:20:00', '青岛', '青岛胶东机场', '2026-01-31 19:00:00', 320, 78, 24);
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
insert into flight_info values ('深航ZH8801', '深圳航空', '青岛', '青岛胶东机场', '2026-02-26 00:10:00', '上海', '上海浦东机场', '2026-02-26 02:40:00', 450, 73, 9);
insert into flight_info values ('海航HU7809', '海南航空', '大连', '大连周水子机场', '2026-02-27 08:20:00', '海口', '海口美兰机场', '2026-02-27 12:50:00', 820, 94, 51);
insert into flight_info values ('厦航MF8901', '厦门航空', '北京', '北京首都机场', '2026-02-28 13:10:00', '厦门', '厦门高崎机场', '2026-02-28 16:00:00', 680, 89, 42);
insert into flight_info values ('山航SC4812', '山东航空', '济南', '济南遥墙机场', '2026-01-02 08:20:00', '青岛', '青岛胶东机场', '2026-01-02 09:50:00', 180, 80, 42);
insert into flight_info values ('上航FM9102', '上海航空', '上海', '上海虹桥机场', '2026-01-02 10:10:00', '北京', '北京首都机场', '2026-01-02 12:40:00', 350, 90, 55);
insert into flight_info values ('中联航KN5977', '中国联合航空', '北京', '北京大兴机场', '2026-01-02 13:30:00', '佛山', '佛山沙堤机场', '2026-01-02 16:00:00', 220, 180, 98);
insert into flight_info values ('首都航空JD5511', '首都航空', '北京', '北京首都机场', '2026-01-02 15:20:00', '杭州', '杭州萧山机场', '2026-01-02 17:40:00', 380, 120, 66);
insert into flight_info values ('祥鹏航空8L9675', '祥鹏航空', '昆明', '昆明长水机场', '2026-01-02 17:00:00', '丽江', '丽江三义机场', '2026-01-02 18:20:00', 150, 70, 33);
insert into flight_info values ('春秋航空9C8988', '春秋航空', '上海', '上海浦东机场', '2026-01-02 19:10:00', '广州', '广州白云机场', '2026-01-02 21:30:00', 199, 200, 105);
insert into flight_info values ('国航CA1345', '中国国航', '北京', '北京首都机场', '2026-01-03 07:10:00', '深圳', '深圳宝安机场', '2026-01-03 10:20:00', 650, 150, 82);
insert into flight_info values ('南航CZ3902', '中国南方航空', '广州', '广州白云机场', '2026-01-03 08:40:00', '成都', '成都双流机场', '2026-01-03 11:10:00', 520, 100, 47);
insert into flight_info values ('东航MU5403', '东方航空', '上海', '上海虹桥机场', '2026-01-03 09:30:00', '重庆', '重庆江北机场', '2026-01-03 12:00:00', 480, 85, 39);
insert into flight_info values ('深航ZH9701', '深圳航空', '深圳', '深圳宝安机场', '2026-01-03 11:20:00', '武汉', '武汉天河机场', '2026-01-03 13:50:00', 420, 75, 28);
insert into flight_info values ('海航HU7605', '海南航空', '海口', '海口美兰机场', '2026-01-03 12:40:00', '三亚', '三亚凤凰机场', '2026-01-03 13:50:00', 120, 60, 25);
insert into flight_info values ('厦航MF8102', '厦门航空', '厦门', '厦门高崎机场', '2026-01-03 14:10:00', '北京', '北京大兴机场', '2026-01-03 17:20:00', 680, 95, 51);
insert into flight_info values ('川航3U8702', '四川航空', '成都', '成都天府机场', '2026-01-03 15:30:00', '西安', '西安咸阳机场', '2026-01-03 17:40:00', 320, 80, 36);
insert into flight_info values ('吉祥HO1305', '吉祥航空', '南京', '南京禄口机场', '2026-01-03 16:50:00', '上海', '上海浦东机场', '2026-01-03 18:20:00', 130, 70, 41);
insert into flight_info values ('山航SC1102', '山东航空', '北京', '北京首都机场', '2026-01-03 18:10:00', '济南', '济南遥墙机场', '2026-01-03 20:00:00', 300, 85, 44);
insert into flight_info values ('上航FM9305', '上海航空', '广州', '广州白云机场', '2026-01-03 19:20:00', '上海', '上海虹桥机场', '2026-01-03 21:40:00', 280, 90, 52);
insert into flight_info values ('中联航KN5888', '中国联合航空', '佛山', '佛山沙堤机场', '2026-01-04 06:30:00', '北京', '北京大兴机场', '2026-01-04 09:00:00', 240, 160, 88);
insert into flight_info values ('首都航空JD5106', '首都航空', '杭州', '杭州萧山机场', '2026-01-04 07:50:00', '北京', '北京首都机场', '2026-01-04 10:10:00', 360, 110, 63);
insert into flight_info values ('祥鹏航空8L8301', '祥鹏航空', '丽江', '丽江三义机场', '2026-01-04 09:20:00', '昆明', '昆明长水机场', '2026-01-04 10:40:00', 160, 65, 29);
insert into flight_info values ('春秋航空9C8899', '春秋航空', '广州', '广州白云机场', '2026-01-04 10:50:00', '上海', '上海浦东机场', '2026-01-04 13:10:00', 219, 180, 92);
insert into flight_info values ('国航CA2207', '中国国航', '沈阳', '沈阳桃仙机场', '2026-01-04 12:20:00', '哈尔滨', '哈尔滨太平机场', '2026-01-04 14:00:00', 380, 75, 33);
insert into flight_info values ('南航CZ6702', '中国南方航空', '海口', '海口美兰机场', '2026-01-04 13:40:00', '广州', '广州白云机场', '2026-01-04 16:10:00', 450, 80, 37);
insert into flight_info values ('东航MU2205', '东方航空', '西安', '西安咸阳机场', '2026-01-04 15:00:00', '郑州', '郑州新郑机场', '2026-01-04 16:30:00', 200, 70, 31);
insert into flight_info values ('深航ZH8203', '深圳航空', '武汉', '武汉天河机场', '2026-01-04 16:20:00', '深圳', '深圳宝安机场', '2026-01-04 18:50:00', 450, 85, 42);
insert into flight_info values ('海航HU7208', '海南航空', '三亚', '三亚凤凰机场', '2026-01-04 17:40:00', '海口', '海口美兰机场', '2026-01-04 18:50:00', 130, 60, 27);
insert into flight_info values ('厦航MF8206', '厦门航空', '北京', '北京大兴机场', '2026-01-04 19:00:00', '厦门', '厦门高崎机场', '2026-01-04 22:10:00', 700, 100, 55);
insert into flight_info values ('川航3U8305', '四川航空', '重庆', '重庆江北机场', '2026-01-05 07:20:00', '成都', '成都天府机场', '2026-01-05 08:50:00', 150, 75, 39);
insert into flight_info values ('吉祥HO1408', '吉祥航空', '上海', '上海浦东机场', '2026-01-05 08:40:00', '南京', '南京禄口机场', '2026-01-05 10:10:00', 140, 65, 32);
insert into flight_info values ('山航SC4901', '山东航空', '青岛', '青岛胶东机场', '2026-01-05 09:50:00', '济南', '济南遥墙机场', '2026-01-05 11:20:00', 190, 80, 43);
insert into flight_info values ('上航FM9402', '上海航空', '北京', '北京首都机场', '2026-01-05 11:00:00', '上海', '上海虹桥机场', '2026-01-05 13:30:00', 360, 95, 58);
insert into flight_info values ('中联航KN5777', '中国联合航空', '北京', '北京大兴机场', '2026-01-05 12:30:00', '温州', '温州龙湾机场', '2026-01-05 15:00:00', 280, 150, 82);
insert into flight_info values ('首都航空JD5603', '首都航空', '上海', '上海浦东机场', '2026-01-05 14:10:00', '三亚', '三亚凤凰机场', '2026-01-05 17:20:00', 850, 120, 67);
insert into flight_info values ('祥鹏航空8L9502', '祥鹏航空', '昆明', '昆明长水机场', '2026-01-05 15:30:00', '贵阳', '贵阳龙洞堡机场', '2026-01-05 17:00:00', 260, 70, 34);
insert into flight_info values ('春秋航空9C8701', '春秋航空', '北京', '北京大兴机场', '2026-01-05 16:50:00', '广州', '广州白云机场', '2026-01-05 19:20:00', 239, 200, 110);
insert into flight_info values ('国航CA1502', '中国国航', '上海', '上海浦东机场', '2026-01-06 07:30:00', '北京', '北京首都机场', '2026-01-06 10:00:00', 370, 140, 78);
insert into flight_info values ('南航CZ3801', '中国南方航空', '深圳', '深圳宝安机场', '2026-01-06 08:50:00', '广州', '广州白云机场', '2026-01-06 10:20:00', 180, 80, 41);
insert into flight_info values ('东航MU5304', '东方航空', '重庆', '重庆江北机场', '2026-01-06 10:00:00', '上海', '上海虹桥机场', '2026-01-06 12:30:00', 500, 85, 40);
insert into flight_info values ('深航ZH9105', '深圳航空', '北京', '北京大兴机场', '2026-01-06 11:20:00', '武汉', '武汉天河机场', '2026-01-06 13:50:00', 430, 75, 35);
insert into flight_info values ('海航HU7406', '海南航空', '杭州', '杭州萧山机场', '2026-01-06 12:40:00', '海口', '海口美兰机场', '2026-01-06 15:50:00', 780, 90, 48);
insert into flight_info values ('厦航MF8403', '厦门航空', '广州', '广州白云机场', '2026-01-06 14:00:00', '厦门', '厦门高崎机场', '2026-01-06 16:20:00', 420, 85, 44);
insert into flight_info values ('川航3U8901', '四川航空', '成都', '成都双流机场', '2026-01-06 15:20:00', '昆明', '昆明长水机场', '2026-01-06 17:50:00', 480, 80, 38);
insert into flight_info values ('吉祥HO1502', '吉祥航空', '广州', '广州白云机场', '2026-01-06 16:40:00', '南京', '南京禄口机场', '2026-01-06 19:00:00', 310, 70, 36);
insert into flight_info values ('山航SC1205', '山东航空', '北京', '北京首都机场', '2026-01-06 18:00:00', '青岛', '青岛胶东机场', '2026-01-06 20:00:00', 320, 85, 46);
insert into flight_info values ('上航FM9501', '上海航空', '深圳', '深圳宝安机场', '2026-01-07 07:10:00', '上海', '上海浦东机场', '2026-01-07 09:40:00', 490, 90, 53);
insert into flight_info values ('中联航KN5666', '中国联合航空', '温州', '温州龙湾机场', '2026-01-07 08:30:00', '北京', '北京大兴机场', '2026-01-07 11:00:00', 300, 160, 95);
insert into flight_info values ('首都航空JD5708', '首都航空', '三亚', '三亚凤凰机场', '2026-01-07 09:50:00', '上海', '上海浦东机场', '2026-01-07 13:00:00', 880, 110, 62);
insert into flight_info values ('祥鹏航空8L8403', '祥鹏航空', '贵阳', '贵阳龙洞堡机场', '2026-01-07 11:10:00', '昆明', '昆明长水机场', '2026-01-07 12:40:00', 240, 65, 30);
insert into flight_info values ('春秋航空9C8602', '春秋航空', '广州', '广州白云机场', '2026-01-07 12:30:00', '北京', '北京大兴机场', '2026-01-07 15:00:00', 229, 190, 102);
insert into flight_info values ('国航CA1603', '中国国航', '哈尔滨', '哈尔滨太平机场', '2026-01-07 13:50:00', '沈阳', '沈阳桃仙机场', '2026-01-07 15:30:00', 360, 75, 34);
insert into flight_info values ('南航CZ6801', '中国南方航空', '广州', '广州白云机场', '2026-01-07 15:10:00', '海口', '海口美兰机场', '2026-01-07 17:40:00', 460, 80, 39);
insert into flight_info values ('东航MU2302', '东方航空', '郑州', '郑州新郑机场', '2026-01-07 16:30:00', '西安', '西安咸阳机场', '2026-01-07 18:00:00', 210, 70, 32);
insert into flight_info values ('深航ZH8301', '深圳航空', '成都', '成都天府机场', '2026-02-01 08:00:00', '深圳', '深圳宝安机场', '2026-02-01 10:30:00', 600, 90, 47);
insert into flight_info values ('海航HU7102', '海南航空', '北京', '北京首都机场', '2026-02-01 09:20:00', '乌鲁木齐', '乌鲁木齐地窝堡机场', '2026-02-01 15:40:00', 1800, 120, 68);
insert into flight_info values ('厦航MF8502', '厦门航空', '深圳', '深圳宝安机场', '2026-02-01 10:40:00', '福州', '福州长乐机场', '2026-02-01 13:10:00', 450, 85, 42);
insert into flight_info values ('川航3U8701', '四川航空', '昆明', '昆明长水机场', '2026-02-01 12:00:00', '重庆', '重庆江北机场', '2026-02-01 14:30:00', 380, 80, 37);
insert into flight_info values ('吉祥HO1701', '吉祥航空', '青岛', '青岛胶东机场', '2026-02-01 13:20:00', '上海', '上海浦东机场', '2026-02-01 15:50:00', 350, 75, 33);
insert into flight_info values ('山航SC4702', '山东航空', '上海', '上海虹桥机场', '2026-02-01 14:40:00', '济南', '济南遥墙机场', '2026-02-01 17:10:00', 320, 85, 44);
insert into flight_info values ('上航FM9601', '上海航空', '成都', '成都双流机场', '2026-02-02 07:30:00', '上海', '上海浦东机场', '2026-02-02 10:00:00', 580, 95, 52);
insert into flight_info values ('中联航KN5555', '中国联合航空', '北京', '北京大兴机场', '2026-02-02 08:50:00', '长沙', '长沙黄花机场', '2026-02-02 11:20:00', 420, 170, 92);
insert into flight_info values ('首都航空JD5802', '首都航空', '南宁', '南宁吴圩机场', '2026-02-02 10:10:00', '广州', '广州白云机场', '2026-02-02 12:40:00', 300, 100, 55);
insert into flight_info values ('祥鹏航空8L9701', '祥鹏航空', '乌鲁木齐', '乌鲁木齐地窝堡机场', '2026-02-02 11:30:00', '北京', '北京首都机场', '2026-02-02 17:50:00', 1900, 110, 61);
insert into flight_info values ('春秋航空9C8501', '春秋航空', '长沙', '长沙黄花机场', '2026-02-02 13:00:00', '北京', '北京大兴机场', '2026-02-02 15:30:00', 399, 180, 95);
insert into flight_info values ('国航CA1702', '中国国航', '杭州', '杭州萧山机场', '2026-02-03 07:10:00', '成都', '成都天府机场', '2026-02-03 09:40:00', 480, 130, 72);
insert into flight_info values ('南航CZ3701', '中国南方航空', '重庆', '重庆江北机场', '2026-02-03 08:30:00', '广州', '广州白云机场', '2026-02-03 11:00:00', 450, 85, 41);
insert into flight_info values ('东航MU5203', '东方航空', '西安', '西安咸阳机场', '2026-02-03 09:50:00', '上海', '上海虹桥机场', '2026-02-03 12:20:00', 420, 80, 38);
insert into flight_info values ('深航ZH9801', '深圳航空', '厦门', '厦门高崎机场', '2026-02-03 11:10:00', '北京', '北京大兴机场', '2026-02-03 14:20:00', 650, 90, 49);
insert into flight_info values ('海航HU7901', '海南航空', '兰州', '兰州中川机场', '2026-02-03 12:30:00', '北京', '北京首都机场', '2026-02-03 15:40:00', 580, 75, 36);
insert into flight_info values ('厦航MF8801', '厦门航空', '上海', '上海浦东机场', '2026-02-03 13:50:00', '厦门', '厦门高崎机场', '2026-02-03 16:20:00', 380, 85, 43);

-- 改签测试数据 (满足出发城市和到达城市相同)
insert into flight_info values ('东航MU5101', '东方航空', '北京', '北京首都机场', '2026-01-01 08:00:00', '上海', '上海浦东机场', '2026-01-01 10:15:00', 135, 85, 45);
insert into flight_info values ('东航MU5103', '东方航空', '北京', '北京大兴机场', '2026-01-01 11:30:00', '上海', '上海虹桥机场', '2026-01-01 13:45:00', 158, 88, 60);

-- 订单数据 (test用户订单最多)
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251215173000001', 1, '东航MU5101', '张三', '123456789012345678', '北京', '上海', '2026-01-01 08:00:00', '2026-01-01 10:15:00', 135, '2025-12-15 17:30:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251220143000002', 1, '国航CA8394', '张三', '123456789012345678', '北京', '广州', '2026-01-01 10:30:00', '2026-01-01 15:00:00', 455, '2025-12-20 14:30:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251222100000003', 1, '深航ZH9404', '张三', '123456789012345678', '深圳', '成都', '2026-01-01 13:10:00', '2026-01-01 16:30:00', 620, '2025-12-22 10:00:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251223153000004', 1, '东航MU2307', '张三', '123456789012345678', '上海', '广州', '2026-01-01 17:15:00', '2026-01-01 19:30:00', 135, '2025-12-23 15:30:00', '已取消');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251224110000005', 1, '厦航MF8302', '张三', '123456789012345678', '厦门', '上海', '2026-01-08 08:15:00', '2026-01-08 10:40:00', 350, '2025-12-24 11:00:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251225160000006', 1, '南航CZ6108', '张三', '123456789012345678', '沈阳', '广州', '2026-01-18 18:00:00', '2026-01-18 21:30:00', 750, '2025-12-25 16:00:00', '已取消');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251218120000007', 2, '深航ZH1610', '王爱丽', '110101199001011234', '北京', '深圳', '2026-01-01 19:00:00', '2026-01-01 22:25:00', 625, '2025-12-18 12:00:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251219140000008', 2, '海航HU7101', '王爱丽', '110101199001011234', '北京', '海口', '2026-01-05 07:00:00', '2026-01-05 11:20:00', 880, '2025-12-19 14:00:00', '已取消');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251220110000009', 3, '川航3U8888', '李波', '320102199203154567', '成都', '重庆', '2026-01-10 11:40:00', '2026-01-10 13:10:00', 180, '2025-12-20 11:00:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251221150000010', 3, '吉祥HO1203', '李波', '320102199203154567', '上海', '南京', '2026-01-12 14:20:00', '2026-01-12 15:50:00', 150, '2025-12-21 15:00:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251222100000011', 3, '东航MU5501', '李波', '320102199203154567', '西安', '上海', '2026-01-20 20:10:00', '2026-01-20 22:45:00', 420, '2025-12-22 10:00:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251223120000012', 4, '海航HU7306', '张查理', '440103199505207890', '杭州', '北京', '2026-01-25 05:30:00', '2026-01-25 08:00:00', 480, '2025-12-23 12:00:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251224140000013', 5, '厦航MF8501', '赵大卫', '510104199208081234', '福州', '深圳', '2026-01-28 10:00:00', '2026-01-28 12:30:00', 520, '2025-12-24 14:00:00', '已支付');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251225160000014', 5, '川航3U6709', '赵大卫', '510104199208081234', '重庆', '昆明', '2026-01-30 13:45:00', '2026-01-30 16:15:00', 650, '2025-12-25 16:00:00', '已取消');
insert into order_info (order_no, user_id, flight_id, passenger_name, passenger_idcard, departure_city, arrival_city, departure_time, arrival_time, price, order_time, order_status) values ('20251226100000015', 7, '吉祥HO1808', '刘弗兰克', '420102199612253456', '南京', '青岛', '2026-01-31 17:20:00', '2026-01-31 19:00:00', 320, '2025-12-26 10:00:00', '已支付');

-- 收藏数据 (test用户收藏)
insert into favorite_info (user_id, flight_id, create_time) values (1, '国航CA1509', '2025-12-20 09:00:00');
insert into favorite_info (user_id, flight_id, create_time) values (1, '南航CZ3405', '2025-12-21 14:30:00');
insert into favorite_info (user_id, flight_id, create_time) values (1, '东航MU2901', '2025-12-22 16:20:00');
