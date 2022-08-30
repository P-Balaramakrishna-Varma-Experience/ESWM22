//write hello world api
const express = require("express");
const bodyParser = require("body-parser");
const cors = require("cors");
const mongoose = require("mongoose");
const PORT = 4000;
const data = require("./data.model");


const app = express();
app.use(cors());
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));


mongoose.connect("mongodb+srv://bala:rXs53OGOucrVbPUb@cluster0.je6s2u8.mongodb.net/?retryWrites=true&w=majority", {
  useNewUrlParser: true,
});

const connection = mongoose.connection; // Make sure that this is not used as a function.
connection.once("open", () => {
  console.log("MongoDB connection success!");
});


app.listen(PORT, () => {
console.log("Server running on port: " + PORT);
});


app.get("/", (req, res) => {
  data.find((error, data) => {
    if(error)
    {
      console.log(error);
      res.send({message: "Not working"})
    }
    else
    {
      res.send({data: data, message: "works"});
    }
  })
}
);


/*
@app.get("/")
def GetState():
    collection = mongo.db.data
    latest = collection.find().sort("_id", pymongo.DESCENDING).limit(1)
    return jsonify(json_util.dumps(latest))


## No checking for parameters
### secutyr defence programming
@app.post("/")
def SetState():
    record = request.json
    collection = mongo.db.data
    collection.insert_one(record)
    return "POST request sent"
    */