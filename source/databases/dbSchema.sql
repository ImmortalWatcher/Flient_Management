-- 创建数据库
create database if not exists flient_management;

use flient_management;

-- 用户表
create table if not exists user_info (
	id int primary key auto_increment comment 'id',
    username varchar(32) not null unique comment '用户名',
    password varchar(32) not null comment '密码'
);

-- 管理员表
create table if not exists admin_info (
    id int primary key auto_increment comment 'id',
    username varchar(32) not null unique comment '用户名',
    password varchar(32) not null comment '密码'
);

-- 测试数据
insert user_info (username, password) values ('test', '123456');
insert admin_info (username, password) values ('admin', '123456');
