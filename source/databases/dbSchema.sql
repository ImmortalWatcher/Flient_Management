-- 创建数据库
create database if not exists flient_management;

use flient_management;

-- 用户表
create table if not exists user_info (
        id int primary key auto_increment comment 'id',
    username varchar(32) not null unique comment '用户名',
    password varchar(32) not null comment '密码',
    role int not null comment '身份'
);

-- 测试数据 (用户)
insert user_info (username, password,role) values ('test', '123456',0);
insert user_info (username,password,role) values ('admin','123456',1);

select * from user_info;
