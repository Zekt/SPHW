all:
	@-cp bidding_system.c bidding_system
	@-chmod +x bidding_system
	@-cp customer.c customer
	@-chmod +x customer
	@echo "gcc bidding_system.c -std=gnu11 -o bidding_system"
	@echo "gcc customer.c -std=gnu11 -o customer"

clean:
	rm --force bidding_system customer *log
