start: initializer consumer producer finisher

initializer: initializer.c
	@echo "Compilando inicializador..."
	gcc -o initializer initializer.c -lrt -lpthread

consumer: consumer.c
	@echo "Compilando consumer..."
	gcc -o consumer consumer.c -lrt -lpthread

producer: producer.c
	@echo "Compilando producer..."
	gcc -o producer producer.c -lrt -lpthread

finisher: finisher.c
	@echo "Compilando finisher..."
	gcc -o finisher finisher.c -lrt -lpthread
