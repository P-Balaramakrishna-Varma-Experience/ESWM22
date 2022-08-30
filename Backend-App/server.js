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

// post sample data of schema data
app.post("/", (req, res) => {
  const to_push = new data({k_p: 0, k_d:0, k_i:0, desired: 40, run: true})
  to_push.save()
  .then((user) => {
      res.send({message: "Sucess", data: data})
  })
  .catch((err) => {
    res.status(404).send({message: "ERR", err: err})
  })
  
})


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