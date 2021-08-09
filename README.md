*server*: make
        ./serv
*client*: gcc -o clie clie.c -lpthread
        ./clie

**notice**: change SERV_IP in clie.c
