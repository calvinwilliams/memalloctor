# 此文件由makeobj.sh自动生成
############################################################
# 项目名 : 
# 模块名 : 
# 备  注 : 
############################################################

###### 源文件配置区
#@ c_FILE
c_FILE		=	\
			test_memalloctor.c \
			test_memlist.c \
			test_memqueue.c \
			test_memtree.c \
			test_memfile.c \

###### 目标文件、安装目录配置区
include ../makeinstall
NOINST_OBJ	=	test_memalloctor \
			test_memlist \
			test_memqueue \
			test_memtree \
			test_memfile \

###### 编译选项
CFLAGS		=	$(_CFLAGS) \
			-I$(_HDERINST) \

###### 链接选项
LFLAGS		=	$(_LFLAGS) \
			-L$(_LIBINST) \
			-lmemalloctor \

###### 额外宏定义区
CLEAN_ADDITION	=	

###### 加载mktpl模板库
#@ make_all
#@ make_clean
#@ make_install
#@ make_install_LIBINST
#@ make_install_HDERINST
#@ make_install_DFTHDERINST
#@ make_uninstall
#@ make_uninstall_LIBINST
#@ make_uninstall_HDERINST
#@ make_uninstall_DFTHDERINST
include $(MKTPLDIR)/makeobj_$(MKTPLOS).inc

###### 目标文件依赖关系
test_memalloctor:	test_memalloctor.o
	$(CC) -o $@ test_memalloctor.o $(LFLAGS)

test_memlist	:	test_memlist.o
	$(CC) -o $@ test_memlist.o $(LFLAGS)

test_memqueue	:	test_memqueue.o
	$(CC) -o $@ test_memqueue.o $(LFLAGS)

test_memtree	:	test_memtree.o
	$(CC) -o $@ test_memtree.o $(LFLAGS)

test_memfile	:	test_memfile.o
	$(CC) -o $@ test_memfile.o $(LFLAGS)

