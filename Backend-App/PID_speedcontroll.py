import collections
from flask import Flask, jsonify
from flask_pymongo import PyMongo

app = Flask(__name__)
app.config["MONGO_URI"] = "mongodb+srv://bala:rXs53OGOucrVbPUb@cluster0.je6s2u8.mongodb.net/ESWM22"
mongo = PyMongo(app)


@app.get("/")
def GetState():
    collection = mongo.db.data
    print(collection)
    latest = collection.find({})#.sort({'_id':-1})#.limit(1);
    a =[]
    for i in latest:
        a.append(i)
    print(latest , dir(latest))
    return jsonify(latest)


@app.post("/")
def SetState():
    return "<p>Hello, World!</p>"