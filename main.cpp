#include "CerberusDB.hpp"
#include <iostream>

int main() {
    // Initialize DB with a low 100-byte flush limit to force SSTable creations
    CerberusDB db(100);

    std::cout << "--- Inserting Items ---\n";
    db.Put("user_101", "Alice");
    db.Put("user_102", "Bob");
    db.Put("user_103", "Charlie"); // Exceeds limit -> triggers flush to disk

    std::cout << "user_101: " << db.Get("user_101").value_or("NOT FOUND") << "\n";
    std::cout << "user_102: " << db.Get("user_102").value_or("NOT FOUND") << "\n";

    std::cout << "\n--- Deleting Item ---\n";
    db.Delete("user_101");
    std::cout << "user_101 after delete: " << db.Get("user_101").value_or("NOT FOUND") << "\n";

    return 0;
}