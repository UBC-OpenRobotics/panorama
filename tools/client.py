import requests

response = requests.get("127.0.0.1:8080")

print(response.text)