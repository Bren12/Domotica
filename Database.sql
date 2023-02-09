DROP DATABASE IF EXISTS domotica_eq5;
CREATE DATABASE domotica_eq5;
USE domotica_eq5;

CREATE TABLE NODEMCU(
	node_id INT(5) NOT NULL,
	node_chip VARCHAR(10) NOT NULL,
	node_user CHAR(20) NOT NULL,
	PRIMARY KEY(node_id)
);

CREATE TABLE TEMPERATURA(
	node_id INT(5) NOT NULL,
	temp_fecha DATETIME NOT NULL,
	temp_temp FLOAT(10) NOT NULL,
	temp_humedad FLOAT(10) NOT NULL,
	FOREIGN KEY(node_id) REFERENCES NodeMCU(node_id)
);

CREATE TABLE LUZ(
    node_id INT(5) NOT NULL,
	luz_fecha DATETIME NOT NULL,
	luz_voltaje FLOAT(10) NOT NULL,
	FOREIGN KEY(node_id) REFERENCES NodeMCU(node_id)
);

CREATE TABLE MOVIMIENTO (
	node_id INT(5) NOT NULL,
	mov_fecha DATETIME NOT NULL,
	mov_dist FLOAT(10) NOT NULL,
	FOREIGN KEY(node_id) REFERENCES NodeMCU(node_id)
);

CREATE TABLE SERVOMOTOR (
	node_id INT(5) NOT NULL,
	ser_fecha DATETIME NOT NULL,
	ser_vel INT(5) NOT NULL,
	FOREIGN KEY(node_id) REFERENCES NodeMCU(node_id)
);

Insert INTO NodeMCU VALUES(12,'ESP8266','Brenda');
Insert INTO NodeMCU VALUES(23,'ESP8266','Karen');
