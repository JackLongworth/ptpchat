#pragma once

#include "framework.h"
#include "client.h"
#include <vector>
#include <mutex>

class ClientList {
public:
	void addClient(const Client& client) {
		std::lock_guard<std::mutex> lock(mtx);
		clients.push_back(client);
	}

	Client getClient(size_t index) const {
		std::lock_guard<std::mutex> lock(mtx);
		return clients[index];
	}

	bool removeClientByIndex(size_t index) {
		std::lock_guard<std::mutex> lock(mtx);
		if (index >= clients.size()) return false;
		clients.erase(clients.begin() + index);
		return true;
	}

	size_t size() const {
		std::lock_guard<std::mutex> lock(mtx);
		return clients.size();
	}

	void clear() {
		std::lock_guard<std::mutex> lock(mtx);
		clients.clear();
	}

	std::vector<Client> snapshot() const {
		std::lock_guard<std::mutex> lock(mtx);
		return clients;
	}

private:
	mutable std::mutex mtx;
	std::vector<Client> clients;
};