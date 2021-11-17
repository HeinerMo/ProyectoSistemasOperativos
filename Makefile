start: initializer consumer producer


initializer: initializer.c
	@echo "Compilando inicializador..."
	gcc -o initializer initializer.c

consumer: consumer.c
	@echo "Compilando consumer..."
	gcc -o consumer consumer.c

producer: producer.c
	@echo "Compilando producer..."
	gcc -o producer producer.c
