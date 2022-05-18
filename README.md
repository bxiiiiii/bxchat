*server*: make
        ./serv
        
*client*: gcc -o clie clie.c -lpthread
        ./clie

**notice**: change SERV_IP in clie.c

# 功能
- 账号管理
	> 登录、注册
        >
	> 找回密码
- 好友管理
	> 私聊（实时性）
        >
	> 添加、删除、查询好友
        >
	> 查看好友列表
        >
	> 显示好友状态
        >
	> 查看聊天记录
        >
	> 屏蔽好友消息
- 群管理
	> 群聊（实时性）
        >
	> 创建、解散群聊
        >
	> 申请加群、退群
        >
	> 查看群成员
        >
	> 查看聊天记录
        >
	> 设置管理员
        >
	> 踢人
- 文件传输
- 关于客户端与服务器
	> 稳定高效，有一定容错处理
        >
	> 处理大量客户的连接与信息处理、传输
	>
        > 日志，记录状态信息

# learn morn 
- [blog](https://blog.csdn.net/binxxi/article/details/119595064?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522165287488616782395384873%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=165287488616782395384873&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-2-119595064-null-null.142^v10^control,157^v4^control&utm_term=binxxi&spm=1018.2226.3001.4187)