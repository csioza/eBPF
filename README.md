一、环境
fedora 31
sudo yum install libbpf-devel make clang llvm elfutils-libelf-devel bpftool bcc-tools bcc-devel net-tools -y
sudo yum install libbpf-static.x86_64 elfutils-libelf-devel-static.x86_64 zlib-static.x86_64 glibc-static.x86_64 -y

二、运行docker
docker run --privileged -itv ~/eBPF:/home/eBPF xxx

三、编译
1. 编译bpf内核态程序
clang -g -O2 -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -I. -c xdp-proxy.bpf.c -o xdp-proxy.bpf.o
2. 生成.h脚手架文件
bpftool gen skeleton xdp-proxy.bpf.o > xdp-proxy.skel.h
3. 编译bpf用户态程序
clang -g -O2 -Wall -I. -c xdp-proxy.c -o xdp-proxy.o
4. 生成静态bpf可执行用户态程序
clang -Wall -O2 -g xdp-proxy.o -static -lbpf -lelf -lz -o xdp-proxy

四、挂载
shell将bpf内核态程序加载到网卡eth0
ip link set dev eth0 xdp obj xdp-drop-world.o sec xdp
即刻时间中，在容器中的虚拟网卡上，只支持以通用模式挂载（xdpgeneric）
ip link set dev eth0 xdpgeneric object xdp-proxy.bpf.o sec xdp

五、分析
# 查看生成的elf格式的可执行文件的相关信息
# 能看到上文提到的Section信息
readelf -a xdp-dproxy.o
# 通过llvm-objdump这个工具来分析下这个可执行文件的反汇编指令信息
llvm-objdump -S xdp-dproxy.o

六、报错&解决
1. /usr/bin/ld: cannot find -lbpf
yum install libbpf-static.x86_64
2. /usr/bin/ld: cannot find -lelf
yum install elfutils-libelf-devel-static.x86_64
3. /usr/bin/ld: cannot find -lz
yum install zlib-static.x86_64
4. /usr/bin/ld: cannot find -lc
yum install glibc-static.x86_64