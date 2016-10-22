#pip install requests
#yum install gcc-c++ unixODBC-devel
#pip install pyodbc

import requests, json, psycopg2

response = requests.post("https://bwackwat.com/api/blog", json={"username":"bwackwat"}, verify=False)
data = json.loads(response.content)

connection = psycopg2.connect("host='localhost' dbname='webservice' user='postgres' password='aq12ws'")
cursor = connection.cursor()

cursor.execute("SELECT id FROM users WHERE username='bwackwat';")
id = cursor.fetchall()[0][0]

cursor.execute("DELETE FROM posts;")
connection.commit()

for item in data["result"]:
	cursor.execute(("INSERT INTO posts "
		"(owner_id, title, content, created_on) "
		"VALUES (%s, %s, %s, %s);"), (id, item["title"], item["content"], item["created_on"]))
	connection.commit()
