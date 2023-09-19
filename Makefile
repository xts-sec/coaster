CC=gcc

SRC_DIR=src
BUILD_DIR=build

.PHONY: always clean coaster

#
# coaster
#
coaster: $(SRC_DIR)/coaster.c always
	$(CC) -g -o $(BUILD_DIR)/coaster $(SRC_DIR)/coaster.c

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
