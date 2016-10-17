import requests, json, pyodbc

response = requests.post("POST", "https://localhost/", json={"username":"bwackwat"})
data = json.loads(response.content)

connection = pyodbc.connect("DRIVER={PostgreSQL Unicode};DATABASE=postgres;UID=postgres;PWD=aq12ws;SERVER=localhost;")
cursor = connection.cursor()

id = cursor.execute("SELECT * FROM users WHERE username='bwackwat' RETURNING id;")
cursor.execute("DELETE * FROM info WHERE id='?';", id)

for item in data:
    cursor.execute(("INSERT INTO info "
                  "(id, data1, data2, data3) "
                  "VALUES (?, ?, ?, ?);"), id, item.data1, item.data2, item.data3)
    connection.commit()