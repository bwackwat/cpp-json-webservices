# Flashlight - Extendable JSON WebService

## Goals

- Obfuscate configuration outside of the project.
- Support PostgreSQL slave replication.
- Make login tokens temporary and distributed via the database.
- Further abstract database layer. Support MongoDB? Abstract SQL away from pqxx?
- Secure routes as non-static member functions?
- Design "WebService" to have "WebService" children, so that routes become hierarchical in a tree.
