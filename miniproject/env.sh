alias stop_server="echo \"STOP\" > /dev/udp/127.0.0.1/9999"
alias start_server="echo \"START\" > /dev/udp/127.0.0.1/9999"
alias ngwcom="minicom -D /dev/ttyS0 --color=on"
alias avr-gcc="~/sola/ttk4147/ex08/buildroot/output/staging/usr/bin/avr32-linux-gcc"
export PATH=$PATH:~/sola/ttk4147/ex08/buildroot/output/staging/usr/bin