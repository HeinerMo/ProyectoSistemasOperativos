start: initializer consumer producer finisher

initializer: initializer.c
	@echo "Compilando inicializador..."
	gcc -o initializer initializer.c -lrt

consumer: consumer.c
	@echo "Compilando consumer..."
	gcc -o consumer consumer.c -lrt

producer: producer.c
	@echo "Compilando producer..."
	gcc -o producer producer.c -lrt

finisher: finisher.c
	@echo "Compilando finisher..."
	gcc -o finisher finisher.c -lrt
