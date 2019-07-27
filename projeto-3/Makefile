# variables
CC=javac
JAR=jar cvf
RUN=java
RM=rm -f
HOSTNAME=localhost
REGISTRY=rmiregistry

all: compute/compute.jar engine/*.class client/*.class

compute/%.class: compute/%.java
	$(CC) compute/$*.java

compute/compute.jar: compute/*.class
	$(JAR) compute/compute.jar compute/*.class

engine/*.class: compute/compute.jar engine/*.java
	$(CC) -cp compute/compute.jar engine/*.java

client/*.class: compute/compute.jar compute/*.java
	$(CC) -cp compute/compute.jar client/*.java

clean:
	$(RM) compute/compute.jar
	$(RM) compute/*.class
	$(RM) engine/*.class
	$(RM) client/*.class

registry:
	$(REGISTRY) &

run-datagen: engine/DataGen.class
	$(RUN) engine/DataGen

run-server: engine/ComputeEngine.class
	$(RUN) engine/ComputeEngine -Djava.rmi.server.hostname=$(HOSTNAME)

run-client: client/Client.class
	$(RUN) client/Client $(HOSTNAME)
