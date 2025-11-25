create database if not exists flient_management;

use flient_management;

create table if not exists user_info (
	id int primary key auto_increment comment 'id',
    user_id int unique not null comment '用户id',
    username varchar(32) null default null comment '用户名',
    password varchar(32) not null comment '密码'
);
