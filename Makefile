# 默认目标
all: application bootloader

# 编译 application
application:
	$(MAKE) -f Makefile.application -j16

# 编译 bootloader
bootloader:
	$(MAKE) -f Makefile.bootloader -j16

# 清理
clean:
	$(MAKE) -f Makefile.application clean
	$(MAKE) -f Makefile.bootloader clean

# 下载
flash-all: flash-bootloader flash-application flash-web-resources

# 下载 bootloader
flash-bootloader:
	$(MAKE) -f Makefile.bootloader flash

# 下载 application
flash-application:
	$(MAKE) -f Makefile.application flash

# 下载 web-resources
flash-web-resources:
	$(MAKE) -f Makefile.application flash-web-resources

bootloader-debug:
	@echo "Building bootloader with debug info..."
	$(MAKE) -f Makefile.bootloader print-debug
	$(MAKE) -f Makefile.bootloader

.PHONY: all application bootloader clean flash-all flash-bootloader flash-application bootloader-debug
