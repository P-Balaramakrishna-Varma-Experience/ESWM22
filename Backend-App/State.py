import bson.json_util as json_util
from flask import Flask, jsonify
from flask_pymongo import PyMongo
import pymongo

app = Flask(__name__)
app.config["MONGO_URI"] = "mongodb+srv://bala:rXs53OGOucrVbPUb@cluster0.je6s2u8.mongodb.net/ESWM22"
mongo = PyMongo(app)


@app.get("/")
def GetState():
    collection = mongo.db.data
    latest = collection.find().sort("_id", pymongo.DESCENDING).limit(1)
    return jsonify(json_util.dumps(latest))


@app.post("/")
def SetState():
    return "<p>Hello, World!</p>"