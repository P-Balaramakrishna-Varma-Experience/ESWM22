//write hello world api
const express = require("express");
const bodyParser = require("body-parser");
const cors = require("cors");
const mongoose = require("mongoose");
const PORT = 4000;
const data = require("./data.model");
//const om2m_data = require("./omdat.model");
const axios = require('axios');

const app = express();
app.use(cors());
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));


mongoose.connect("mongodb://localhost:27017", {
  useNewUrlParser: true,
});

/*mongoose.connect("mongodb+srv://bala:rXs53OGOucrVbPUb@cluster0.je6s2u8.mongodb.net/?retryWrites=true&w=majority", {
  useNewUrlParser: true,
});*/

/*mongoose.connect("mongodb+srv://esp:0rP4zdXDIdevDdis@cluster0.je6s2u8.mongodb.net/?retryWrites=true&w=majority", {
  useNewUrlParser: true,
});
*/

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

app.get("/latest", async(req, res) => {
  try {
    const om2m_response = await axios({
      method: 'get',
      url: 'http://127.0.0.1:5089/~/in-cse/in-name/PID_control_of_DC_motor_speed/Node-1/Data/la',
      headers: {
        'X-M2M-Origin': 'admin:admin',
        'Accept': 'application/json'
      }
    });
    let data = JSON.parse(om2m_response.data["m2m:cin"]["con"]);
    res.status(200).send({ data: data, message: "works" });
  }
  catch (error) {
    console.log(error);
    res.status(400).send({ message: "Not working there is a problem or there are no documents", error: error });
  }
})

app.get("/latestiiith", async(req, res) => {
  try {
    const om2m_response = await axios({
      method: 'get',
      url: 'https://esw-onem2m.iiit.ac.in/~/in-cse/in-name/Team-33/Node-1/Data/la',
      headers: {
        'X-M2M-Origin': '2Z@d!E:rpPXgt',
        'Accept': 'application/json'
      }
    });
    let data = JSON.parse(om2m_response.data["m2m:cin"]["con"]);
    res.status(200).send({ data: data, message: "works" });
  }
  catch (error) {
    console.log(error);
    res.status(400).send({ message: "Not working there is a problem or there are no documents", error: error });
  }
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
    data.push(JSON.parse(cin["con"]))
  })

  let flat_data = data.flat(Infinity)
  res.status(200).send({data: flat_data})
})

app.get("/om2m/reset", async(req, res) => {
  try {
    let del_response = await axios({
      method: 'delete',
      url: 'http://127.0.0.1:5089/~/in-cse/in-name/PID_control_of_DC_motor_speed/Node-1/Data/',
      headers: {
        'X-M2M-Origin': 'admin:admin',
        'Accept': 'application/json'
      }
    }) 
  }
  catch (err)
  {
    res.status(404).send({message: "deletion does not work", err: err})
  }

  try {
    let create_response = await axios({
      method: 'post',
      url: 'http://127.0.0.1:5089/~/in-cse/in-name/PID_control_of_DC_motor_speed/Node-1/',
      headers: {
        'X-M2M-Origin': 'admin:admin',
        'Content-Type': 'application/json;ty=3'
      },
      data: {
        "m2m:cnt":{
          "rn": "Data",
          "lbl": [
            "Label-1",
            "Label-2"
          ],
          "mni": 100
        }
      }
    })
  }
  catch(err)
  {
    res.status(404).send({message: "creating does not work", err: err})
  }

  res.status(200).send({message: "works"})
})

//onem2m we send and receive string data
//FRONTEND needs integer data to plot but I am sending string data.. and it is working so far