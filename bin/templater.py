import os, sys

print "Arguments: %s" % sys.argv

TEMPLATE_NAME = "template.html"
dir = "./"

if len(sys.argv) > 1:
	dir = sys.argv[1]
	os.stat(dir)

def process_template(dir):
	template_file = os.path.join(dir, TEMPLATE_NAME)
	print "Template: " + template_file
	with open(template_file, "r+") as template:
		template_text = template.read()
		for root, dirs, files in os.walk(dir):
			for file in files:
				file = os.path.join(dir, file)
				if file.endswith(".template.html"):
					new_file_name = file.replace(".template.html", ".html")
					print "Templating {CONTENT} in %s to %s" % (file, new_file_name)
					with open(file, "r+") as oldfile, open(new_file_name, "w+") as newfile:
						newfile.write(template_text.replace("{CONTENT}", oldfile.read()))

for root, dirs, files in os.walk(dir):
	for file in files:
		if file == TEMPLATE_NAME:
			process_template(root)

print "DONE!"
