//write hello world api
const express = require("express");
const bodyParser = require("body-parser");
const cors = require("cors");
const mongoose = require("mongoose");
const PORT = 4000;
const data = require("./data.model");
const axios = require('axios');

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
    if (error) {
      console.log(error);
      res.send({ message: "Not working there is a problem or there are no documents" })
    }
    else {
      res.send({ data: data, message: "works" });
    }
  })
}
);

// post sample data of schema data
app.post("/", async (req, res) => {
  const to_push = new data({ k_p: req.body.k_p, k_d: req.body.k_d, k_i: req.body.k_i, desired: req.body.desired, run: req.body.run})
  let doc = await data.findOneAndUpdate({}, {k_p: req.body.k_p, k_d: req.body.k_d, k_i: req.body.k_i, desired: req.body.desired, run: req.body.run})
  if(doc)
    res.send({ message: "works"});
  else
    res.send({ message: "error"});
})

//verify manually if error or not
app.post("/init", async(req, res) => {
  const to_push = new data({ k_p: req.body.k_p, k_d: req.body.k_d, k_i: req.body.k_i, desired: req.body.desired, run: req.body.run})
  await to_push.save();
  res.send({ message: "works"});
})

app.get("/del", async(req, res) => {
  await data.remove({});
  res.send({ message: "works"});
})

app.get("/visual", async(req, res) => {
  let data = [];
  let len = Math.random() * 40 % 40;
  for(let i = 0; i < len; i++)
    data.push(Math.random());
  res.send({ data: data, message: "works"});
})

app.get("/visual2", async(req, res) =>{
  const om2m_response = await axios({
    method: 'get',
    url: 'http://127.0.0.1:5089/~/in-cse/in-name/PID_control_of_DC_motor_speed/Node-1/Data?rcn=4',
    headers: {
      'X-M2M-Origin': 'admin:admin',
      'Accept': 'application/json'
    }
  })

  let data = []
  om2m_response.data["m2m:cnt"]["m2m:cin"].forEach((cin) => {
    data.push(cin["con"])
  })

  res.status(200).send({data: data})
})
