import sys
import dateutil.parser
import psycopg2
from BeautifulSoup import BeautifulSoup

try:
	conn = psycopg2.connect("dbname='webservice' user='postgres' password='aq12ws'")
	cur = conn.cursor()
except:
	print "bad db connection"

reload(sys)
sys.setdefaultencoding('utf8')

posts = BeautifulSoup(open("friendly-adventure/blog/posts.html", "r").read())
res = posts.find("div")
resultsFile = open("results.txt", "w+")

for post in res.contents:
	if post.name == "hr":
		continue
	title = post.contents[0].text
	date = post.contents[1].text
	truedate = dateutil.parser.parse(date)
	content = "".join(map(str, post.contents[3].contents))

	cur.execute("INSERT INTO posts (owner_id, title, content, created_on) VALUES (%s, %s, %s, %s)", (2, title, content, str(truedate)))

	resultsFile.write(title + " | " + date + "\n")
	resultsFile.write(content + "\n\n")

conn.commit()
cur.close()

resultsFile.close()
