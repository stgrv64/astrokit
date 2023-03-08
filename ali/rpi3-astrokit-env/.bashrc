# ~/.bashrc: executed by bash(1) for non-login shells.

NC="\[\e[0m\]"
NOIR="\[\e[30;1m\]"
VERT="\[\e[32;40m\]" #"\033[0;32;40m"
JAUNE="\[\e[33;40m\]" #"\033[0;33;40m"

#export PS1='\h:\w\$ '

export PATH=$PATH:$HOME/git/astrokit:/usr/local/bin
export PS1="$NC\u@\H:$JAUNE\w($OS) $VERT>$NC "

umask 022

# You may uncomment the following lines if you want `ls' to be colorized:
# export LS_OPTIONS='--color=auto'
# eval `dircolors`
# alias ls='ls $LS_OPTIONS'
# alias ll='ls $LS_OPTIONS -l'
# alias l='ls $LS_OPTIONS -lA'
#
# Some more alias to avoid making mistakes:
# alias rm='rm -i'
# alias cp='cp -i'
# alias mv='mv -i'
