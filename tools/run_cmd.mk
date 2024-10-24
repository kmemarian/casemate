Q ?= @

INDENT = \t

define run_cmd
	@printf "  "%-8s"$(INDENT)%s\n" '$1' '$2' 2>&1
	$(Q)$3
endef

define run_clean
	$(call run_cmd,CLEAN,$1, \
		rm -f $2 \
	)
endef

define run_cc
$(call run_cmd,CC,$1, \
		echo $(CC) $(CFLAGS) -c $1 -o $2 > $2.cmd \
		; $(CC) $(CFLAGS) -c $1 -o $2 \
	)
endef

define run_cc_as_ld
$(call run_cmd,LD,$2, \
		$(CC) $(CFLAGS) $1 -o $2 \
	)
endef

define run_ld
$(call run_cmd,LD,$2, \
		$(LD) $(LDFLAGS) $1 -o $2 \
	)
endef

define run_objcopy
$(call run_cmd,OBJCOPY,$2, \
		$(OBJCOPY) $(OBJCOPYFLAGS) $1 -o $2 \
	)
endef

define run_cp
$(call run_cmd,COPY,$2, \
		cp $1 $2 \
	)
endef
