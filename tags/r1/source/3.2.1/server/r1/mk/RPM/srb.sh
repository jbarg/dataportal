# SRB settings

if echo $SHELL |grep bash 2>&1 >/dev/null; then # aliases are bash only
  # System wide functions and aliases
  alias chksrb='ps -elf | grep "srb[S|M]" | grep -v grep; /sbin/service srb status'
fi

