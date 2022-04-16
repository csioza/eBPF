# 一、环境
fedora 31
sudo yum install libbpf-devel make clang llvm elfutils-libelf-devel bpftool bcc-tools bcc-devel net-tools -y
sudo yum install libbpf-static.x86_64 elfutils-libelf-devel-static.x86_64 zlib-static.x86_64 glibc-static.x86_64 -y

# 二、运行docker
docker run --privileged -itv ~/eBPF:/home/eBPF xxx

# 三、编译
1. 编译bpf内核态程序
clang -g -O2 -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -I. -c xdp-proxy.bpf.c -o xdp-proxy.bpf.o
clang -g -O2 -target bpf -c xdp-proxy.bpf.c -o xdp-proxy.bpf.o
2. 生成.h脚手架文件
bpftool gen skeleton xdp-proxy.bpf.o > xdp-proxy.skel.h
3. 编译bpf用户态程序
clang -g -O2 -Wall -I. -c xdp-proxy.c -o xdp-proxy.o
4. 生成静态bpf可执行用户态程序
clang -Wall -O2 -g xdp-proxy.o -static -lbpf -lelf -lz -o xdp-proxy

# 四、挂载
shell将bpf内核态程序加载到网卡eth0
ip link set dev eth0 xdp obj xdp-drop-world.o sec xdp
即刻时间中，在容器中的虚拟网卡上，只支持以通用模式挂载（xdpgeneric）
ip link set dev eth0 xdpgeneric object xdp-proxy.bpf.o sec xdp

# 五、分析
1. 查看生成的elf格式的可执行文件的相关信息能看到上文提到的Section信息`readelf -a xdp-dproxy.o`

2. 通过llvm-objdump这个工具来分析下这个可执行文件的反汇编指令信息
llvm-objdump -S xdp-dproxy.o

# 六、报错&解决
1. /usr/bin/ld: cannot find -lbpf
yum install libbpf-static.x86_64
2. /usr/bin/ld: cannot find -lelf
yum install elfutils-libelf-devel-static.x86_64
3. /usr/bin/ld: cannot find -lz
yum install zlib-static.x86_64
4. /usr/bin/ld: cannot find -lc
yum install glibc-static.x86_64

# 七、新手教程

1. bpf_map_update_elem函数有四个参数。
第一个指向已定义映射的指针。
第二个是指向要更新的键的指针。因为内核不知道更新键的类型，所以该参数定义为指向void的不透明指针，这意味着我们可以传入任意数据。
第三个参数是我们要存入的值。此参数使用与键参数相同的语义。随后我们会有更多示例来说明如何使用不透明指针。
第四个参数是更新映射的方式，此参数有三个值：
如果传递0，表示如果元素存在，内核将更新元素；如果不存在，则在映射中创建该元素。
如果传递1，表示仅在元素不存在时，内核创建元素。
如果传递2，表示仅在元素存在时，内核更新元素。
为方便记忆，三个值被定义成常量。如BPF_ANY表示0，BPF_NOEXIST表示1，BPF_EXIST表示2。



# 编译
`clang -g -O2 -target bpf -c xdp-proxy.bpf.c -o xdp-proxy.bpf.o`
# eBPF/XDP
### 加载
`ip link set dev eth0 xdp obj xdp-proxy.bpf.o sec xdp`
### 卸载
`ip link set dev eth0 xdp off`
## eBPF/TC
### 查看排队规则
`tc qdisc show dev eth0`
### 添加排队规则
`tc qdisc add dev eth0 clsact`
### 加载
`tc filter add dev eth0 ingress bpf da obj xdp-proxy.bpf.o sec rx`

`tc filter add dev eth0 egress bpf da obj xdp-proxy.bpf.o sec rx`

`tc filter show dev eth0 ingress`

`tc filter show dev eth0 egress`

### 卸载
`tc filter del dev eth0 ingress`

`tc filter del dev eth0 egress`

### 基础工具安装
`yum install -y net-tools`

`yum install -y iproute`

`yum install -y iproute-tc`