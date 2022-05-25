# ~/.profile: executed by the command interpreter for login shells.
# This file is not read by bash(1), if ~/.bash_profile or ~/.bash_login
# exists.
# see /usr/share/doc/bash/examples/startup-files for examples.
# the files are located in the bash-doc package.

# the default umask is set in /etc/profile; for setting the umask
# for ssh logins, install and configure the libpam-umask package.
#umask 022

# if running bash
if [ -n "$BASH_VERSION" ]; then
    # include .bashrc if it exists
    if [ -f "$HOME/.bashrc" ]; then
	. "$HOME/.bashrc"
    fi
fi

# set PATH so it includes user's private bin if it exists
if [ -d "$HOME/bin" ] ; then
    PATH="$HOME/bin:$PATH"
fi

# set PATH so it includes user's private bin if it exists
if [ -d "$HOME/.local/bin" ] ; then
    PATH="$HOME/.local/bin:$PATH"
fi

ipdelllent=192.168.43.153
ip=192.168.43.82
alias ssh_dell_lent='ssh stef@$ipdelllent'
alias arp-local='sudo arp-scan --localnet'
alias arp-rpi-locate='sudo arp-scan --localnet | grep Rasp'
alias arp_scan_hon_hai_precision='sudo arp-scan --localnet | grep -i "Hon Hai Precision"'
alias arp_scan_hon_hai_precision_ip="sudo arp-scan --localnet | grep -i \"Hon Hai Precision\" | awk '{print \$1}'"
