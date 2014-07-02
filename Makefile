all:
	$(MAKE) -C iop           install
	$(MAKE) -C ee            install
	$(MAKE) -C iop           copy
	$(MAKE) -C sample_client copy

clean:
	$(MAKE) -C iop           clean
	$(MAKE) -C ee            clean
	$(MAKE) -C sample_client clean
