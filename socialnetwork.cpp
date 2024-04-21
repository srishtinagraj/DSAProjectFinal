#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <set>
#include <algorithm>

class User {
public:
    std::string name;
    std::string handle;

    User() : name(""), handle("") {}

    User(std::string name, std::string handle) : name(name), handle(handle) {}
};

class SocialNetwork {
private:
    std::map<int, std::vector<int>> graph;
    std::map<int, User> users;

public:
    void addUser(int userId, std::string name, std::string handle) {
        users[userId] = User(name, handle);
        graph[userId] = {};
    }

    void addConnection(int user1, int user2) {
        graph[user1].push_back(user2);
        graph[user2].push_back(user1);
    }

    std::vector<int> getConnections(int userId) {
        return graph[userId];
    }

    void displayConnectionsRecursive(int userId, int level = 0) {
        if (level > 2) return; // Limit depth to 2
        std::string prefix(4 * level, ' ');
        std::string connectionPrefix = level > 0 ? "|-- " : "";
        std::cout << prefix << connectionPrefix << users[userId].name << " (" << users[userId].handle << ")" << std::endl;
        
        auto connections = getConnections(userId);
        for (int connId : connections) {
            displayConnectionsRecursive(connId, level + 1);
        }
    }

    void suggestFriends(int userId) {
        std::set<int> directFriends(graph[userId].begin(), graph[userId].end());
        std::map<int, int> mutualFriendsCount;

        for (int friendId : directFriends) {
            for (int potentialFriendId : graph[friendId]) {
                if (potentialFriendId != userId && directFriends.find(potentialFriendId) == directFriends.end()) {
                    mutualFriendsCount[potentialFriendId]++;
                }
            }
        }

        std::vector<std::pair<int, int>> sortedMutualFriends(mutualFriendsCount.begin(), mutualFriendsCount.end());
        std::sort(sortedMutualFriends.begin(), sortedMutualFriends.end(), 
                  [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                      return a.second > b.second;
                  });

        std::cout << "Suggested friends for " << users[userId].name << " based on mutual connections:" << std::endl;
        for (auto& mf : sortedMutualFriends) {
            std::cout << " - " << users[mf.first].name << " (" << users[mf.first].handle << "), Mutual Friends: " << mf.second << std::endl;
        }
        if (sortedMutualFriends.empty()) {
            std::cout << "No suggestions available." << std::endl;
        }
    }

    void showInfluentialUsers() {
        std::vector<std::pair<int, int>> influence;
        for (auto& user : graph) {
            influence.push_back(std::make_pair(user.first, (int)user.second.size()));
        }
        std::sort(influence.begin(), influence.end(), 
                  [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                      return a.second > b.second;
                  });

        std::cout << "Influential users based on the number of connections:" << std::endl;
        for (auto& user : influence) {
            std::cout << " - " << users[user.first].name << " (" << users[user.first].handle << ") with " << user.second << " connections." << std::endl;
        }
    }

    int getUserIdByHandle(std::string handle) {
        for (auto& user : users) {
            if (user.second.handle == handle) {
                return user.first;
            }
        }
        return -1; // Not found
    }
};

void loadFromCSV(SocialNetwork& network, const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return;
    }

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> rowData;
        while (getline(ss, item, ',')) {
            rowData.push_back(item);
        }
        if (rowData.size() < 3) continue;

        int userId = std::stoi(rowData[0]);
        std::string name = rowData[1];
        std::string handle = rowData[2];
        network.addUser(userId, name, handle);

        for (size_t i = 3; i < rowData.size(); ++i) {
            int connectionId = std::stoi(rowData[i]);
            network.addConnection(userId, connectionId);
        }
    }

    file.close();
}

int main() {
    SocialNetwork sn;
    loadFromCSV(sn, "users.csv");

    std::string handle;
    char choice;

    do {
        std::cout << "\nWhat would you like to do today?\n";
        std::cout << "1: Find a user's connections\n";
        std::cout << "2: Get friend suggestions\n";
        std::cout << "3: See influential users in your circle\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch(choice) {
            case '1':
                std::cout << "Enter the handle to search: ";
                std::cin >> handle;
                {
                    int userId = sn.getUserIdByHandle(handle);
                    if (userId != -1) {
                        sn.displayConnectionsRecursive(userId);
                    } else {
                        std::cout << "User handle not found." << std::endl;
                    }
                }
                break;
            case '2':
                std::cout << "Enter your handle to get friend suggestions: ";
                std::cin >> handle;
                {
                    int userId = sn.getUserIdByHandle(handle);
                    if (userId != -1) {
                        sn.suggestFriends(userId);
                    } else {
                        std::cout << "User handle not found." << std::endl;
                    }
                }
                break;
            case '3':
                sn.showInfluentialUsers();
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
        std::cout << "\nDo you want to continue? (Y/N): ";
        std::cin >> choice;
    } while (choice == 'Y' || choice == 'y');

    return 0;
}
